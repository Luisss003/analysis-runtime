#ifndef METADATA_H
#define METADATA_H

#include <sys/types.h>
#include "pointer_analysis.h"

//Allocation Metadata
typedef enum {
    UNKNOWN,
    LIVE,
    FREED,
}ALLOC_STATE;

typedef struct AllocationGeneration {
    size_t generation;
    void *usable_base;
    size_t requested_size;
    int creation_event_id; //The event ID that created this generation
    int free_event_id; //The event ID that freed this generation 
    ALLOC_STATE state;
    //ChunkMetadata *chunk_meta; //Associated chunk metadata
    PointerMetadata pointers[10];
} AllocationGeneration;

///////////////////////////////////////////////////////////////////
//Chunk Metadata
enum CHUNK_ORIGIN {
    MALLOC,
    CALLOC,
    REALLOC,
    MMAP,
    UNKNOWN_ORIGIN,
};

enum CHUNK_STATE {
    CHUNK_LIVE,
    CHUNK_TCACHE,
    CHUNK_FASTBIN,
    CHUNK_UNSORTED_BIN,
    CHUNK_SMALLBIN,
    CHUNK_LARGEBIN,
    CHUNK_TOP,
};

enum CHUNK_END {
    CONSOLIDATED,
    SPLIT,
    TOP,
    UNMMAP,
};

typedef struct {
    enum CHUNK_STATE state;
    void *base;
    size_t size; //range = [base, base + size)
    enum CHUNK_ORIGIN origin;
    enum CHUNK_STATE chunk_state;
    size_t id;
    enum CHUNK_END end;
    struct AllocationGeneration *alloc_gen; //Associated allocation generation
} ChunkMetadata ;

extern int current_generation;
extern AllocationGeneration alloc_gens[100];

#endif