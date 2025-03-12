#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

#define MEMORY_ALIGNMENT 16
#define HEAP_CAPACITY 8192

void *my_malloc(size_t size, char *file, int line);
void my_free(void *ptr, char *file, int line);

void init_heap(void);
size_t adjust_size(size_t size);
void check_leaks(void);

#endif 
