#include "pointer_analysis.h"
#include "alloc_metadata.h"
#include <stdlib.h>
#include <stdio.h>

PointerMetadata ptr_metadata[100];
int ptr_md_idx = 0;
int id = 0;

/**
 * Upon pointer creation, associate it with created generation.
 */
PointerMetadata ptr_preprocess(AllocationGeneration *gen, void *addr, void *ptr_location){
    PointerMetadata ptr;
    ptr.id = id;
    ptr.generation = gen->generation;
    ptr.addr = addr;
    ptr.ptr_location = ptr_location;

    fprintf(stderr, "Pointer Struct Created: id: %d, gen: %d, points_to: %p, pointer_location: %p\n", 
                    ptr.id,
                    ptr.generation,
                    ptr.addr,
                    ptr.ptr_location);
                    
    id++;
    return ptr;
}

void ptr_copy(void *src, void *dst){
    /**
     * A pointer copy is when we directly copy the value/address that ptr A points-to
     * into a second pointer B
     * 
     * int *x = malloc(4*sizeof(int));
     * int *z = x;
     * 
     * Because they point to the same generation, intuitively z should recieve
     * x's provenance, or origin.
     */


}
