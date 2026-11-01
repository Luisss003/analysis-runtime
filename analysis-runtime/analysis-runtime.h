#ifndef ANALYSIS_RUNTIME_H
#define ANALYSIS_RUNTIME_H

#include <sys/types.h>

void record_malloc(void *ptr, size_t size, void *ptr_location);
void record_free(void *ptr);

#endif