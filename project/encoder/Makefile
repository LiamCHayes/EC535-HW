CC = arm-linux-gnueabihf-gcc
CFLAGS = -std=c99 -Wall -I. -static -O0
DEPS = 
OBJ1 = read_encoder.o
OBJ2 = encoder_i2c_conf.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

read_encoder: $(OBJ1)
	$(CC) -o $@ $^ $(CFLAGS)
	
encoder_i2c_conf: $(OBJ2)
	$(CC) -o $@ $^ $(CFLAGS)
	
clean:
	rm *.o read_encoder encoder_i2c_conf
