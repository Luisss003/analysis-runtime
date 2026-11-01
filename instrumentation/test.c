#include <stdlib.h>
#include <unistd.h>

int main(void) {
	char *a = malloc(8);
	char *c = malloc(-1);
	free(a);
	char *b = malloc(8);
	free(NULL);
	return 0;
}


