#ifndef METADATA_TYPES_H
#define METADATA_TYPES_H

#include <stddef.h>

typedef struct  {
    int id;
    int generation; //The generation that the pointer is associated with
    void *points_to; //addr the pointer points to
    void *location; //The address of where the pointer resides
    int reassign_flag; //Determine if a pointer was reassigned to a new value in its lifetime
} PointerMetadata;

//Allocation Metadata
typedef enum ALLOC_STATE {
    UNKNOWN,
    LIVE,
    FREED,
} ALLOC_STATE;

#endif