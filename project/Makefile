# --- Toolchain and Compiler Flags ---
CC = arm-linux-gnueabihf-gcc
TARGET = inv_pend

CFLAGS = -std=c99 -Wall -O0
INCLUDE_FLAGS = -static -I./include

# --- Source and Object Files ---
# All .c files in the src directory
SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.c) 

OBJ_DIR = obj
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Build the final executable
$(TARGET): $(OBJ_FILES)
	@echo "Linking executable: $(TARGET)"
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDE_FLAGS)

# Make the objects directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDE_FLAGS)

clean:
	@echo "Cleaning up build files..."
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)
