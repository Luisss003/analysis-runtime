
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

// Resolve the storage location from which a pointer
// value originated, when directly observable.
//
// This handles:
//   %v = load ptr, ptr %a
//   %v = select i1 %cond, ptr %x, ptr %y
//
// It does NOT yet handle PHI nodes or function returns.
static Value *getSourceLocation(
    Value *V,
    IRBuilder<> &Builder
) {
    V = V->stripPointerCasts();

    if (auto *Load = dyn_cast<LoadInst>(V)) {

        if (!Load->getType()->isPointerTy())
            return nullptr;

        return Load->getPointerOperand();
    }

    if (auto *Select = dyn_cast<SelectInst>(V)) {

        Value *TrueSource = getSourceLocation(
            Select->getTrueValue(), Builder
        );

        Value *FalseSource = getSourceLocation(
            Select->getFalseValue(), Builder
        );

        if (!TrueSource || !FalseSource)
            return nullptr;

        // Select the source storage location that
        // corresponds to the selected pointer value.
        return Builder.CreateSelect(
            Select->getCondition(),
            TrueSource,
            FalseSource
        );
    }

    return nullptr;
}

static bool isMallocResult(Value *V) {
    V = V->stripPointerCasts();

    auto *Call = dyn_cast<CallBase>(V);

    if (!Call)
        return false;

    Function *Callee = Call->getCalledFunction();

    return Callee &&
           Callee->getName() == "malloc";
}

/**
 * Pointer Arithmetic could have an expression like: ptr + 2 + 5,
 * so we cant blindly accept the first GEP we see
 */
static Value *getGEPBase(Value *V) {

    while (true) {
        V = V->stripPointerCasts();

        auto *GEP = dyn_cast<GetElementPtrInst>(V);

        if (!GEP)
            return V;

        V = GEP->getPointerOperand();
    }
}


struct CallScan : PassInfoMixin<CallScan> {

    PreservedAnalyses run(
        Module &M,
        ModuleAnalysisManager &
    ) {
        LLVMContext &Ctx = M.getContext();

        Type *VoidTy = Type::getVoidTy(Ctx);
        Type *I64Ty = Type::getInt64Ty(Ctx);
        auto *PtrTy = PointerType::getUnqual(Ctx);

        FunctionCallee record_malloc =
            M.getOrInsertFunction(
                "record_malloc",
                FunctionType::get(
                    VoidTy,
                    {PtrTy, I64Ty, PtrTy},
                    false
                )
            );

        FunctionCallee record_free =
            M.getOrInsertFunction(
                "record_free",
                FunctionType::get(
                    VoidTy,
                    {PtrTy},
                    false
                )
            );

        FunctionCallee ptr_shallow_copy =
            M.getOrInsertFunction(
                "ptr_shallow_copy",
                FunctionType::get(
                    VoidTy,
                    {PtrTy, PtrTy, PtrTy},
                    false
                )
            );

        FunctionCallee ptr_arithmetic = 
        M.getOrInsertFunction(
            "ptr_arithmetic",
            FunctionType::get(
                VoidTy,
                {PtrTy, PtrTy, PtrTy, PtrTy},
                false
            )
        );

        SmallVector<Instruction *, 128> WorkList;

        // Store all instructs in module for processing.
        for (Function &F : M) {
            if (F.isDeclaration())
                continue;

            for (BasicBlock &BB : F) {
                for (Instruction &I : BB)
                    WorkList.push_back(&I);
            }
        }

        for (Instruction *I : WorkList) {

            if (auto *Store = dyn_cast<StoreInst>(I)) {

                Value *Stored = Store->getValueOperand();

                if (!Stored->getType()->isPointerTy())
                    continue;

                if (Stored->getType()
                        ->getPointerAddressSpace() != 0)
                    continue;

                Value *Destination =
                    Store->getPointerOperand();

                if (Destination->getType()
                        ->getPointerAddressSpace() != 0)
                    continue;

                // malloc's initial storage association
                // is handled by record_malloc.
                if (isMallocResult(Stored))
                    continue;

                Instruction *Next = Store->getNextNode();

                if (!Next)
                    continue;

                IRBuilder<> Builder(Next);

                // -----------------------------------
                // Pointer arithmetic propagation
                // -----------------------------------
                Value *Stripped = Stored->stripPointerCasts();

                if (auto *GEP =
                        dyn_cast<GetElementPtrInst>(Stripped)) {

                    // Trace through consecutive GEPs.
                    Value *OriginalValue =
                        getGEPBase(GEP->getPointerOperand());

                    // Find the original pointer variable's
                    // storage location.
                    Value *SourceLocation =
                        getSourceLocation(OriginalValue, Builder);

                    if (SourceLocation) {

                        Builder.CreateCall(
                            ptr_arithmetic,
                            {
                                SourceLocation,
                                OriginalValue,
                                Stored,
                                Destination
                            }
                        );

                    } else {

                        // No known source storage location.
                        // Invalidate any prior destination
                        // association using existing behavior.
                        Builder.CreateCall(
                            ptr_shallow_copy,
                            {
                                ConstantPointerNull::get(PtrTy),
                                Stored,
                                Destination
                            }
                        );
                    }

                    // Do not also record a shallow copy.
                    continue;
                }

                // -----------------------------------
                // Ordinary pointer shallow copies
                // -----------------------------------
                Value *Source = getSourceLocation(
                    Stored, Builder
                );

                if (!Source)
                    Source = ConstantPointerNull::get(PtrTy);

                Builder.CreateCall(
                    ptr_shallow_copy,
                    {
                        Source,
                        Stored,
                        Destination
                    }
                );

                continue;
            }


            // -----------------------------------
            // malloc / free Calls
            // -----------------------------------
            //Extract name of function called from instruct
            auto *Call = dyn_cast<CallInst>(I);

            if (!Call)
                continue;

            Function *Callee = Call->getCalledFunction();

            if (!Callee)
                continue;

            StringRef Name = Callee->getName();

            if (Name == "malloc") {
                Instruction *Next = Call->getNextNode();

                if (!Next)
                    continue;

                Value *PtrLocation =
                    ConstantPointerNull::get(PtrTy);

                Instruction *InsertBefore = Next;

                // Handle the straightforward pattern:
                //
                //   %p = call ptr @malloc(...)
                //   store ptr %p, ptr %a
                //
                if (auto *Store = dyn_cast<StoreInst>(Next)) {

                    if (Store->getValueOperand() == Call) {

                        PtrLocation =
                            Store->getPointerOperand();

                        // Record after the initial store.
                        InsertBefore = Store->getNextNode();

                        if (!InsertBefore)
                            continue;
                    }
                }

                IRBuilder<> Builder(InsertBefore);

                Builder.CreateCall(
                    record_malloc,
                    {
                        Call,
                        Call->getArgOperand(0),
                        PtrLocation
                    }
                );
            }
            else if (Name == "free") {

                // Record the attempt before execution.
                IRBuilder<> Builder(Call);

                Builder.CreateCall(
                    record_free,
                    {
                        Call->getArgOperand(0)
                    }
                );
            }
        }

        return PreservedAnalyses::none();
    }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK
PassPluginLibraryInfo llvmGetPassPluginInfo() {

    return {
        LLVM_PLUGIN_API_VERSION,
        "CallScan",
        LLVM_VERSION_STRING,

        [](PassBuilder &PB) {

            PB.registerPipelineParsingCallback(
                [](StringRef Name,
                   ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {

                    if (Name != "call-scan")
                        return false;

                    MPM.addPass(CallScan());
                    return true;
                }
            );
        }
    };
}
