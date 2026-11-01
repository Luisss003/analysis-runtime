
CLANG := clang-19
OPT := opt-19

PLUGIN := build/libCallScan.so
PASS_SOURCES := $(wildcard *.cpp *.h)
RUNTIME := ../analysis-runtime/runtime.c ../analysis-runtime/event_log.c ../analysis-runtime/pointer_analysis.c
RUNTIME_HEADERS := $(wildcard ../analysis-runtime/*.h)

.PHONY: all run clean

all: test-instrumented

# 1. Rebuild LLVM pass when its source changes
$(PLUGIN): $(PASS_SOURCES) CMakeLists.txt
	cmake --build build --parallel

# 2. Compile test.c to LLVM bitcode
test.bc: test.c
	$(CLANG) -O0 -fno-builtin \
		-Xclang -disable-O0-optnone \
		-emit-llvm -c test.c -o test.bc

# 3. Instrument bitcode using the LLVM pass
instrumented.bc: test.bc $(PLUGIN)
	$(OPT) -load-pass-plugin=$(PLUGIN) \
		-passes=call-scan test.bc -o instrumented.bc

# 4. Link instrumented program with runtime
test-instrumented: instrumented.bc $(RUNTIME) $(RUNTIME_HEADERS)
	$(CLANG) instrumented.bc $(RUNTIME) -o test-instrumented

# 5. Execute instrumented program
run: test-instrumented
	./test-instrumented

# 6. Remove generated files
clean:
	rm -f test.bc instrumented.bc test-instrumented

