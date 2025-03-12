#include <stdio.h>
#include "mymalloc.h"

int main(void) {
    char *buffer = malloc(50);
    if (buffer) {
        free(buffer);
    }
    return 0;
}
