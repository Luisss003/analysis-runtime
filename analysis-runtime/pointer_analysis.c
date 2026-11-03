#include "pointer_analysis.h"
#include "alloc_metadata.h"
#include "event_log.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
                   
    ptr_metadata[id] = ptr;
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
void ptr_shallow_copy(void *orig_location, void *copy_value, void *copy_location){
    //1. Find original pointer metadata
    PointerMetadata *orig_ptr = NULL;
    for(int i = 0; i < id; i++){
        if(ptr_metadata[i].location == orig_location){
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

    //2. Check if we are creating a new pointer, or reassigning an existing pointer
    for(int i = 0; i < id; i++){
        if (ptr_metadata[i].location == copy_location){
            fprintf(stderr, "Reassigned\n");
            //Pointer reassigned, so update stored metadata with provenance of original/src pointer
            ptr_metadata[i].generation = orig_ptr->generation;
            ptr_metadata[i].points_to = orig_ptr->points_to;
            ptr_metadata[i].reassign_flag = 1;
            write_event_log(POINTER_PROVENANCE_PROPOGATION, orig_ptr, &ptr_metadata[i]);
            return;
        }
    }
    fprintf(stderr, "No reassign\n");
    PointerMetadata copy_ptr;
    copy_ptr.id = id;
    copy_ptr.generation = orig_ptr->generation;
    copy_ptr.points_to = orig_ptr->points_to;
    copy_ptr.location = copy_location;
    copy_ptr.reassign_flag = 0;

    //3. Store the new pointer metadata in the global array
    if(memcpy(&ptr_metadata[id], &copy_ptr, sizeof(copy_ptr)) == NULL) {
        exit(0);
    }
    write_event_log(POINTER_PROVENANCE_PROPOGATION, orig_ptr, &ptr_metadata[id]);
    
    id++;
}

void ptr_arithmetic( void *src_location, void *src_value, void *derived_value, 
    void *dst_location) {
        //1. Find the original pointer metadata
        PointerMetadata *src_metadata = resolve_metadata(src_location, src_value);
        if(src_metadata == NULL) {
            fprintf(stderr, "ptr_arithmetic fail\n");
            exit(0);
        }

        /**
         * 2. Handle inc/dec OR self reassignment (i.e a = a + 4)
         * Simply update metadata
         */
        if(src_location == dst_location) {
            //Create of old metadata for event logging
            PointerMetadata old_metadata = *src_metadata;
            src_metadata->points_to = derived_value;
            src_metadata->reassign_flag = 1;
            write_event_log(POINTER_ARITHMETIC, &old_metadata, src_metadata);
            return;
        }

        //3. Create new metadata for derived pointer
        PointerMetadata derived_metadata;
        derived_metadata.id = id;
        derived_metadata.generation = src_metadata->generation;
        derived_metadata.points_to = derived_value;
        derived_metadata.location = dst_location;
        derived_metadata.reassign_flag = 0;

        //4. Store the new pointer metadata in the global array
        if(memcpy(&ptr_metadata[id], &derived_metadata, sizeof(derived_metadata)) == NULL) {
            exit(0);
        }
        write_event_log(POINTER_ARITHMETIC, src_metadata, &ptr_metadata[id]);
        
        id++;
}

PointerMetadata *resolve_metadata(void *location, void *value) {
    for(int i = 0; i < id; i++){
        if(ptr_metadata[i].location == location &&
            ptr_metadata[i].points_to == value){
            return &ptr_metadata[i];
        }
    }
    return NULL;
}