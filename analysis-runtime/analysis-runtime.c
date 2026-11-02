#include "analysis-runtime.h"
#include "alloc_metadata.h"
#include <stdio.h>
#include <sys/types.h>
#include "pointer_analysis.h"
#include <unistd.h>
#include <stdlib.h>

#include "event_log.h"

int current_generation = 0;
AllocationGeneration alloc_gens[100];


void record_malloc(void *ptr, size_t size, void *ptr_location){
    //Check for failed malloc, and if so, log it and return
    if(ptr == NULL){
        write_event_log(MALLOC_FAILED, NULL, NULL);
        return;
    }
    if((size_t)current_generation >= sizeof alloc_gens / sizeof alloc_gens[0]){
        fprintf(stderr, "Allocation generation metadata capacity exceeded\n");
        exit(EXIT_FAILURE);
    }

    //1. Check if ptr using reclaimed chunk or potentially overlapping alloc
    int i = 0;
    int reuse_flag = 0;
    for(i = current_generation - 1; i >= 0; i--){
        if(alloc_gens[i].usable_base == ptr){
            if(alloc_gens[i].state == FREED){
                reuse_flag = 1;
                break;
            }
            else if(alloc_gens[i].state == LIVE){
                write_event_log(MALLOC_OVERLAP, ptr, NULL);
                break;
            }
            else{
                fprintf(stderr, "Unknown allocation state for generation %d\n", i);
                exit(EXIT_FAILURE);
            }
        }
    }

    //2. Update generation metadata
    alloc_gens[current_generation].generation = current_generation;
    alloc_gens[current_generation].usable_base = ptr;
    alloc_gens[current_generation].requested_size = size;
    alloc_gens[current_generation].state = LIVE;
    alloc_gens[current_generation].free_event_id = -1;

    //3. Associate ptr with new generation
    PointerMetadata pd = preprocess(&alloc_gens[current_generation], ptr, ptr_location);

    for(int i = 0; i < 10; i++){
        if(alloc_gens[current_generation].pointers[i].points_to == NULL){
            alloc_gens[current_generation].pointers[i] = pd;
            break;
        }
    }

    //4. Write allocation generation creation to event log, and store log ID
    if(reuse_flag == 0){
        alloc_gens[current_generation].creation_event_id = write_event_log(GENERATION_CREATION, &alloc_gens[current_generation], NULL);
    }
    else{
        alloc_gens[current_generation].creation_event_id = write_event_log(GENERATION_CREATION_REUSE, &alloc_gens[current_generation], NULL);
    }
    
    current_generation++; 
}

/**
 * Records a free call for the given pointer.
 * The generation associated with the pointer is marked as FREED.
 * If the pointer is not found in the allocation generations, log a warning message.
 */
void record_free(void *ptr){
    //1. Check if arg to free() is NULL or its a non-heap pointer
    if(ptr == NULL) {
       //Log the free, but clarify no generation was altered
       write_event_log(FREE_NULL, NULL, NULL);
       return;
    }


    //1. If a valid call to free(), starting at last popualted metadata index,
    //look for entry with matching base address
    //This works because free ONLY works with an allocations base; not within the alloc itself
    for(int i = current_generation - 1; i >= 0; i--){
        //Found our newest generation (ASSUMES NO OVERLAP, SO FIX THAT LATER)
        if(alloc_gens[i].usable_base == ptr){
            if(alloc_gens[i].state == LIVE){
                alloc_gens[i].state = FREED;
                alloc_gens[i].free_event_id = write_event_log(GENERATION_FREE, &alloc_gens[i], NULL);
                return;
            }
            else if(alloc_gens[i].state == FREED){
                //Log the free, but clarify no generation was altered
                write_event_log(FREE_DOUBLE, ptr, NULL);
                return;
            }
            else{
                fprintf(stderr, "Unknown allocation state for generation %d\n", i);
                exit(EXIT_FAILURE);
            }
            return;
        }
    }

    //If nothing matches, likely passed a fake allocation or stack addr, so log it
    write_event_log(FREE_INVALID, ptr, NULL);
}
