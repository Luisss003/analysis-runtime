#ifndef EVENT_LOG_H
#define EVENT_LOG_H

#include <stddef.h>
#include <stdint.h>


typedef enum  {
    GENERATION_CREATION,
    GENERATION_CREATION_REUSE,
    GENERATION_FREE,
    FREE_NULL,
    FREE_DOUBLE,
    MALLOC_OVERLAP,
    POINTER_PROVENANCE_PROPOGATION,
    MALLOC_FAILED,
    FREE_INVALID,
    POINTER_COPY,
    POINTER_ALLOCATION,
    POINTER_ARITHMETIC
} EventType;

int write_event_log(EventType event_type, void *ptr, void *ptr2);
void read_event_log();
void close_event_log();
int open_event_log();

extern int event_log_fd;
extern int event_id;

#endif 
