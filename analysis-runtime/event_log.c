#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <malloc.h>
#include <string.h>

#include "event_log.h"
#include "alloc_metadata.h"

int event_log_fd = -1;
int event_id = 0;

int open_event_log(){
    event_log_fd = open("event_log.bin", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if(event_log_fd == -1){
        perror("Failed to open event log file");
        exit(0);
    }
    return event_log_fd;
}

//Pass string 
int write_event_log(EventType event_type, void *ptr){
    if(event_log_fd == -1){
        open_event_log();
    }

    char final_log[256];

    if(event_type == GENERATION_CREATION){
        AllocationGeneration *alloc_gen = (AllocationGeneration *)ptr;
        int written = snprintf(final_log, 255 ,"[EventType=c,EventID=%d,GenerationID=%zu,Base=%p,RequestedSize=%zu]\n", 
            event_id, 
            alloc_gen->generation, 
            alloc_gen->usable_base, 
            alloc_gen->requested_size);
   
        if(write(event_log_fd, final_log, written) == -1){
            perror("Failed to write to event log file");
            exit(0);
        }

    }
    else if(event_type == GENERATION_CREATION_REUSE){
        AllocationGeneration *alloc_gen = (AllocationGeneration *)ptr;
        int written = snprintf(final_log, 255 ,"[EventType=r,EventID=%d,GenerationID=%zu,Base=%p,RequestedSize=%zu]\n", 
            event_id, 
            alloc_gen->generation, 
            alloc_gen->usable_base, 
            alloc_gen->requested_size);
   
        if(write(event_log_fd, final_log, written) == -1){
            perror("Failed to write to event log file");
            exit(0);
        }

    }
    else if(event_type == GENERATION_FREE){
        AllocationGeneration *alloc_gen = (AllocationGeneration *)ptr;
        int written = snprintf(final_log, 255 ,"[EventType=f,EventID=%d,GenerationID=%zu,Base=%p,RequestedSize=%zu]\n", 
                    event_id, 
                    alloc_gen->generation, 
                    alloc_gen->usable_base, 
                    alloc_gen->requested_size);
   
        if(write(event_log_fd, final_log, written) == -1){
            perror("Failed to write to event log file");
            exit(0);
        } 
    }
    else if(event_type == FREE_NULL){
        int written = snprintf(final_log, 255, "[EventType=fn,EventID=%d]\n",
                    event_id);
        if(write(event_log_fd, final_log, written) == -1){
            perror("Failed to write to event log file");
            exit(0);
        } 
    }
    else if(event_type == MALLOC_FAILED){
        int written = snprintf(final_log, 255, "[EventType=mf,EventID=%d]\n",
                    event_id);
        if(write(event_log_fd, final_log, written) == -1){
            perror("Failed to write to event log file");
            exit(0);
        } 
    }
    else if(event_type == FREE_INVALID){
        int written = snprintf(final_log, 255, "[EventType=fi,EventID=%d]\n",
                    event_id);
        if(write(event_log_fd, final_log, written) == -1){
            perror("Failed to write to event log file");
            exit(0);
        } 
    }
    int ret = event_id;
    event_id++;
    return ret;
}

