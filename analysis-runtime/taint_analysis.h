#ifndef TAINT_ANALYSIS_H
#define TAINT_ANALYSIS_H

#include "sanitizer/dfsan_interface.h"
#include <sys/types.h>

extern const dfsan_label STDIN_DERIVED_LABEL;
void record_read(void *ptr, ssize_t bytes_read);

#endif