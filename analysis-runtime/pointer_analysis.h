#ifndef POINTER_ANALYSIS_H
#define POINTER_ANALYSIS_H

struct AllocationGeneration;

typedef struct  {
    int id;
    int generation; //The generation that the pointer is associated with
    void *points_to; //addr the pointer points to
    void *location; //The address of where the pointer resides
} PointerMetadata;

typedef enum {
    PTR_COPY_DECLARATION_ASSIGNMENT = 1,
    PTR_COPY_REASSIGNMENT = 2,
    PTR_COPY_POINTER_CAST = 3,
    PTR_COPY_VOID_CONVERSION = 4,
    PTR_COPY_POINTER_ARRAY = 5,
    PTR_COPY_STRUCT_FIELD_ASSIGNMENT = 6,
    PTR_COPY_FUNCTION_ARGUMENT = 7,
    PTR_COPY_FUNCTION_RETURN = 8,
    PTR_COPY_CONDITIONAL_ASSIGNMENT = 9
} PTR_COPY_TYPE;
extern int id;

PointerMetadata preprocess(struct AllocationGeneration *gen, void *points_to, void *location);

void ptr_store(void *src, void *dst);
void ptr_load(void *src, void *dst);

void ptr_copy(void *src, void *dst);
void ptr_aliasing(void *src, void *dst);
void ptr_aritmetic(void *src, void *dst);
void ptr2int(void *ptr);
void int2ptr(int ptr);
void ptr_casting(void *ptr);

#endif
