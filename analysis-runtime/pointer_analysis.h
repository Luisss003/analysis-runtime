#ifndef POINTER_ANALYSIS_H
#define POINTER_ANALYSIS_H

#include <sys/types.h>
#include "metadata_types.h"

struct AllocationGeneration;

extern int id;

PointerMetadata preprocess(struct AllocationGeneration *gen, void *points_to, void *location);
void ptr_shallow_copy(void *orig_location, void *copy_value, void *copy_location);
void ptr_arithmetic(void *src_location, void *src_value, void *derived_value, void *dst_location);

void ptr_aliasing(void *src, void *dst);
void ptr2int(void *ptr);
void int2ptr(int ptr);
void ptr_casting(void *ptr);

void mem_read(void *src_location, void *src_value, void *dst_location, void *dst_value, size_t read_size);

PointerMetadata *resolve_metadata(void *location, void *value);
ALLOC_STATE resolve_state(size_t generation);
#endif
