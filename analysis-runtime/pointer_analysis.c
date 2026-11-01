#include "pointer_analysis.h"
#include "alloc_metadata.h"
#include <stdlib.h>
#include <stdio.h>

PointerMetadata ptr_metadata[100];
int id = 0;

/**
 * Upon a call to maloc(), we take the generation, and associate a "PointerMetadata" with it. 
 * This metadata will be used to track the provenance of pointers that point to this generation.
 */

//Return struct itself to avoid use after scope
PointerMetadata preprocess(AllocationGeneration *gen, void *points_to, void *location){
    PointerMetadata ptr;
    ptr.id = id;
    ptr.generation = gen->generation;
    ptr.points_to = points_to;
    ptr.location = location;
                    
    id++;
    return ptr;
}   

/**
 * Creating a copy of a ptr means the copy recieves the provenance of the original pointer.
 * 
 * We need to consider every variant of a pointer copy, including:
 * 1. Decleration assignment: int *a = b;
 * 2. Ressignment: a = b;
 * 3. Pointer Cast: int *a = (int *)b;
 * 4. Void Conversion: void *a = (void *)b;
 * 5. Pointer Array: int *a[10]; a[0] = b;
 * 6. Struct Field Assignment: struct A { int *a; }; struct A s; s.a = b;
 * 7. Function Argument: void foo(int *a); foo(b);
 * 8. Function Return: int *foo(); int *a = foo();
 * 9. Conditional Assignment: int *a = (cond) ? b : c;
 */
void ptr_shallow_copy(void *orig, void *copy, void *copy_location, PTR_COPY_TYPE copy_type){
    //1. Find original pointer metadata
    PointerMetadata *orig_ptr = NULL;
    for(int i = 0; i < id; i++){
        if(ptr_metadata[i].location == orig){
            orig_ptr = &ptr_metadata[i];
            break;
        }

    }

    //If the original ptr MD isn't found, its likely not associated with the heap, so ignore.
    //TODO: Add mmap + munmap support since those are other ways of heap allocations.
    if(orig_ptr == NULL){
        printf("Error: original pointer metadata not found\n");
        return;
    }

    //2. Create a new pointer metadata for the copy
    PointerMetadata copy_ptr;
    copy_ptr.id = id;
    copy_ptr.generation = orig_ptr->generation;
    copy_ptr.points_to = orig_ptr->points_to;


    id++;
}
