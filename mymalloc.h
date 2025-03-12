#ifndef MYMALLOC_H
#define MYMALLOC_H

#include <stddef.h>

void *my_malloc(size_t size, char *file, int line);
void my_free(void *ptr, char *file, int line);

#define malloc(size) my_malloc(size, __FILE__, __LINE__)
#define free(ptr) my_free(ptr, __FILE__, __LINE__)

#endif /* MYMALLOC_H */
