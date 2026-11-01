#ifndef POINTER_ANALYSIS_H
#define POINTER_ANALYSIS_H

/**
 * This file defines the metadata structures we'll maintain in order to keep track of pointer provenance and generation origin
 * as well as the functions to update the structures as generations become stale, pointers are copied, and arithmetic is performed.
 * 
 * Note: We want to instrument all pointer operations BUT, we only want to actually use our 
 * analysis functions if and only if one of the operands is in our metadata. For our topic,
 * we get no use out of for example, tracking the pointers in:
 * 
 * int x = 10;
 * int *z = &x;
 * 
 * since they are not associated with the heap. Therefore, each of our functions must start with
 * a call to a "preprocessor function" that checks to ensure that at least one of the pointers
 * involved in the operation are related to the heap. If not, we simply return without doing anyhting.
 */

struct AllocationGeneration;

typedef struct  {
    int id;
    int generation; //The generation that the pointer is associated with
    void *addr; //addr the pointer points to
    void *ptr_location; //The address of where the pointer resides
} PointerMetadata;

extern int id;

PointerMetadata ptr_preprocess(struct AllocationGeneration *gen, void *addr, void *ptr_location);

void ptr_copy(void *src, void *dst);
void ptr_aliasing(void *src, void *dst);
void ptr_aritmetic(void *src, void *dst);
void ptr2int(void *ptr);
void int2ptr(int ptr);
void ptr_casting(void *ptr);

void ptr_store(void *src, void *dst);
void ptr_load(void *src, void *dst);

#endif
