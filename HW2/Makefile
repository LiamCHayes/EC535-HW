# Define compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Define executable name
TARGET = myISS

# Default target to build the program
all: $(TARGET)

# Link the executable from the object file
$(TARGET): $(TARGET).o
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).o

# Compile the C source file into an object file
$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) -c $(TARGET).c

# Clean rule to remove compiled files
clean:
	rm -f $(TARGET) $(TARGET).o

