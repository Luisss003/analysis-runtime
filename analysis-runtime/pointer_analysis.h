#ifndef POINTER_ANALYSIS_H
#define POINTER_ANALYSIS_H

struct AllocationGeneration;

typedef struct  {
    int id;
    int generation; //The generation that the pointer is associated with
    void *points_to; //addr the pointer points to
    void *location; //The address of where the pointer resides
    int reassign_flag; //Determine if a pointer was reassigned to a new value in its lifetime
} PointerMetadata;

extern int id;

PointerMetadata preprocess(struct AllocationGeneration *gen, void *points_to, void *location);
void ptr_shallow_copy(void *orig_location, void *copy_value, void *copy_location);
void ptr_arithmetic(void *src_location, void *src_value, void *derived_value, void *dst_location);

void ptr_aliasing(void *src, void *dst);
void ptr2int(void *ptr);
void int2ptr(int ptr);
void ptr_casting(void *ptr);

PointerMetadata *resolve_metadata(void *location, void *value);

#endif
