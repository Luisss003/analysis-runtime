
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct CallScan : PassInfoMixin<CallScan> {

    PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
        LLVMContext &Ctx = M.getContext();

        FunctionType *record_malloc_Type = FunctionType::get(
            Type::getVoidTy(Ctx),
            {
                PointerType::getUnqual(Ctx),
                Type::getInt64Ty(Ctx),
                PointerType::getUnqual(Ctx),
            },
            false
        );

        FunctionType *record_free_Type = FunctionType::get(
            Type::getVoidTy(Ctx),
            {
                PointerType::getUnqual(Ctx)
            },
            false
        );

        FunctionCallee record_malloc = M.getOrInsertFunction("record_malloc", record_malloc_Type);
        FunctionCallee record_free = M.getOrInsertFunction("record_free", record_free_Type);

        // Iterate through every function in the module.
        for (Function &F : M) {
            if (F.isDeclaration())
                continue;
            // Iterate through each basic block.
            for (BasicBlock &BB : F) {
                // Iterate through each instruction.
                for (Instruction &I : BB) {
                    auto *Call = dyn_cast<CallBase>(&I);
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

                        // Default: pointer storage location is unknown.
                        Value *PtrLocation = ConstantPointerNull::get(
                            PointerType::getUnqual(Ctx)
                        );

                        Instruction *InsertBefore = Next;

                        // Check whether the next instruction stores malloc's result.
                        if (auto *Store = dyn_cast<StoreInst>(Next)) {

                            // Confirm that this store uses our malloc return value.
                            if (Store->getValueOperand() == Call) {

                                // Destination address of the store.
                                PtrLocation = Store->getPointerOperand();

                                // Instrument after the store has executed.
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

                    else if(Name == "free") {
                        Instruction *Next = Call->getNextNode();
                        if (!Next)
                            continue;
                        IRBuilder<> Builder(Next);

                        Builder.CreateCall(
                            record_free,
                            {
                                Call->getArgOperand(0)
                            }
                        );
                    }


                }
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
