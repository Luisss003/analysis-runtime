#include <malloc.h>

int main(void) {
    int *p = malloc(sizeof(int));
    p = p+1;
    free(p);
    return 0;
}