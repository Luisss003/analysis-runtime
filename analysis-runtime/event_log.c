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

int write_event_log(EventType event_type, void *ptr, void *ptr2){
    if(event_log_fd == -1){
        open_event_log();
    }
    char final_log[256];
    char ptr_str[256] = {0};

    if (event_type == GENERATION_CREATION) {
        AllocationGeneration *alloc_gen = (AllocationGeneration *)ptr;

        size_t offset = 0;
        int first = 1;

        int written = snprintf(ptr_str, sizeof(ptr_str), "Pointers=[");
        if (written < 0 || (size_t)written >= sizeof(ptr_str))
            exit(0);

        offset += (size_t)written;

        for (int i = 0; i < 10; i++) {
            void *ptr_local = alloc_gen->pointers[i].location;

            if (ptr_local == NULL)
                continue;

            written = snprintf(
                ptr_str + offset,
                sizeof(ptr_str) - offset,
                "%s%p",
                first ? "" : ",",
                ptr_local
            );

            if (written < 0 ||
                (size_t)written >= sizeof(ptr_str) - offset)
                exit(0);

            offset += (size_t)written;
            first = 0;
        }

        written = snprintf(
            ptr_str + offset,
            sizeof(ptr_str) - offset,
            "]"
        );

        if (written < 0 ||
            (size_t)written >= sizeof(ptr_str) - offset)
            exit(0);

        int log_written = snprintf(
            final_log,
            sizeof(final_log),
            "[EventType=c,EventID=%d,GenerationID=%zu,"
            "Base=%p,RequestedSize=%zu,%s]\n",
            event_id,
            alloc_gen->generation,
            (void *)alloc_gen->usable_base,
            alloc_gen->requested_size,
            ptr_str
        );

        if (log_written < 0 || (size_t)log_written >= sizeof(final_log)) {
            fprintf(stderr, "Event log formatting failed or was truncated\n");
            return -1;
        }

        if (write(event_log_fd, final_log, (size_t)log_written) == -1) {
            perror("Failed to write to event log file");
            return -1;
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
    else if(event_type == FREE_DOUBLE){
        int written = snprintf(final_log, 255, "[EventType=fd,EventID=%d]\n",
                    event_id);
        if(write(event_log_fd, final_log, written) == -1){
            perror("Failed to write to event log file");
            exit(0);
        } 
    }
    else if(event_type == MALLOC_OVERLAP){
        int written = snprintf(final_log, 255, "[EventType=mo,EventID=%d]\n",
                    event_id);
        if(write(event_log_fd, final_log, written) == -1){
            perror("Failed to write to event log file");
            exit(0);
        } 
    }
    else if(event_type == POINTER_PROVENANCE_PROPOGATION) {
        PointerMetadata *src = (PointerMetadata *)ptr;
        PointerMetadata *dst = (PointerMetadata *)ptr2; 
        
        int written = snprintf(final_log, 255 ,"[EventType=pp,EventID=%d,OriginalPointer=%p,CopyPointer=%p]\n", 
            event_id, 
            src->location,
            dst->location);
        if(write(event_log_fd, final_log, written) == -1){
            perror("Failed to write to event log file");
            exit(0);
        }
        fprintf(stderr, "Pass?\n");
    }
    else{
        fprintf(stderr, "Unknown event type: %d\n", event_type);
        exit(0);
    }
    int ret = event_id;
    event_id++;
    return ret;
}

