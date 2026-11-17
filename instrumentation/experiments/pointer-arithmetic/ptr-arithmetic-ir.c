//clang-19 -O0 -fno-builtin -Xclang -disable-O0-optnone -S -emit-llvm ptr-arithmetic-ir.c -o ptr-arithmetic-ir.ll
#include <malloc.h> 
int main(){
    int *ptr = (int *)malloc(sizeof(int));
    int *ptr2 = (int *)malloc(sizeof(int));
    ptr+ptr2;
    return 0;
}