#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define META_SIZE sizeof(MetaHeader)

static union {
    char memory[HEAP_CAPACITY];
    double dummy;  
} myHeap;

typedef struct {
    size_t blockSize : 63;
    size_t freeFlag  : 1;
} MetaHeader;

static int heapInitialized = 0;

static MetaHeader create_header(size_t blockSize, int isFree) {
    MetaHeader header;
    header.blockSize = blockSize;
    header.freeFlag = isFree;
    return header;
}

void check_leaks(void) {
    size_t totalLeaked = 0;
    int leakCount = 0;
    MetaHeader *ptr = (MetaHeader *)myHeap.memory;
    while ((char *)ptr < myHeap.memory + HEAP_CAPACITY) {
        if (!ptr->freeFlag) {
            totalLeaked += ptr->blockSize;
            leakCount++;
        }
        ptr = (MetaHeader *)((char *)ptr + META_SIZE + ptr->blockSize);
    }
    if (leakCount > 0) {
        fprintf(stderr, "Allocator leak: %zu bytes in %d block(s) still allocated.\n",
                totalLeaked, leakCount);
    }
}

size_t adjust_size(size_t size) {
    return (size + (MEMORY_ALIGNMENT - 1)) & ~(MEMORY_ALIGNMENT - 1);
}

void init_heap(void) {
    *(MetaHeader *)myHeap.memory = create_header(HEAP_CAPACITY - META_SIZE, 1);
    heapInitialized = 1;
    atexit(check_leaks);
}

void *my_malloc(size_t size, char *file, int line) {
    if (!heapInitialized) {
        printf("Initializing heap\n");
        init_heap();
    }
    size = adjust_size(size);
    MetaHeader *current = (MetaHeader *)myHeap.memory;
    while ((char *)current < myHeap.memory + HEAP_CAPACITY) {
        if (current->freeFlag && current->blockSize >= size) {
            size_t leftover = current->blockSize - size - META_SIZE;
            current->freeFlag = 0;
            current->blockSize = size;
            if (leftover >= META_SIZE + MEMORY_ALIGNMENT) {
                MetaHeader *nextBlock = (MetaHeader *)((char *)current + META_SIZE + size);
                *nextBlock = create_header(leftover, 1);
            }
            return (char *)current + META_SIZE;
        }
        current = (MetaHeader *)((char *)current + META_SIZE + current->blockSize);
    }
    fprintf(stderr, "my_malloc: Allocation of %zu bytes failed (%s:%d)\n", size, file, line);
    return NULL;
}

void my_free(void *ptr, char *file, int line) {
    if (!ptr || ptr < (void *)myHeap.memory || ptr >= (void *)(myHeap.memory + HEAP_CAPACITY)) {
        fprintf(stderr, "my_free: Invalid pointer (%s:%d)\n", file, line);
        exit(2);
    }
    MetaHeader *blockHeader = (MetaHeader *)((char *)ptr - META_SIZE);
    if (blockHeader->freeFlag) {
        fprintf(stderr, "my_free: Double free detected at %p (%s:%d)\n", ptr, file, line);
        exit(2);
    }
    blockHeader->freeFlag = 1;

    MetaHeader *nextBlock = (MetaHeader *)((char *)blockHeader + META_SIZE + blockHeader->blockSize);
    if ((char *)nextBlock < myHeap.memory + HEAP_CAPACITY && nextBlock->freeFlag) {
        blockHeader->blockSize += META_SIZE + nextBlock->blockSize;
    }
    
    MetaHeader *iter = (MetaHeader *)myHeap.memory;
    MetaHeader *prevBlock = NULL;
    while ((char *)iter < (char *)blockHeader) {
        prevBlock = iter;
        iter = (MetaHeader *)((char *)iter + META_SIZE + iter->blockSize);
    }
    if (prevBlock && prevBlock->freeFlag) {
        prevBlock->blockSize += META_SIZE + blockHeader->blockSize;
    }
}
