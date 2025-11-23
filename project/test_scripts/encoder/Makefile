CC = arm-linux-gnueabihf-gcc
CFLAGS = -std=c99 -Wall -I. -static -O0
DEPS =
OBJ1 = read_encoder.o
OBJ2 = encoder_i2c_conf.o
OBJS = $(OBJ1) $(OBJ2)

# Default target
all: read_encoder encoder_i2c_conf

# Rule for building object files
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Rule for linking read_encoder
read_encoder: $(OBJ1)
	$(CC) -o $@ $^ $(CFLAGS)

# Rule for linking encoder_i2c_conf
encoder_i2c_conf: $(OBJ2)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm *.o read_encoder encoder_i2c_conf
