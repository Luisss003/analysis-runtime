#include <malloc.h>

int main(void) {
    char *p = malloc(32);
    free(p);
    char *q = malloc(32);
    q[0] = 'a'; // This is a stale write, should be logged as such
    char a = *p; // This is a stale read, should be logged as such
    return 0;
}