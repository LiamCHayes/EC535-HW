# Liam Hayes - liamh

# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g

# Target executable
TARGET = MyBitApp

# Source and object files
SRCS = main.c bits.c mylist.c
OBJS = $(SRCS:.c=.o)

# Build the executable
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)
