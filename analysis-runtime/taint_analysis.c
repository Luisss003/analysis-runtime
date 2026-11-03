#include "taint_analysis.h"
#include "sanitizer/dfsan_interface.h"
#include <stdio.h>
#include <sys/types.h>

const dfsan_label STDIN_DERIVED_LABEL = 1;

void record_read(void *ptr, ssize_t bytes_read){
    if(bytes_read <= 0){
        return;
    }

    dfsan_set_label(STDIN_DERIVED_LABEL, ptr, (size_t)bytes_read);
}