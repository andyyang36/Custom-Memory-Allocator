CC = gcc
CFLAGS = -Wall -Wextra -g  # Add any other compiler flags here
TARGET = myallocator

# Source files for the project
SRCS = main.c allocator.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c mymalloc.h allocator.h

allocator.o: allocator.c allocator.h

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
