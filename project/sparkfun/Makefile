CC = arm-linux-gnueabihf-gcc
CFLAGS = -std=c99 -Wall -I. -static -O0
DEPS = 
OBJ = read_imu.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

read_imu: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	
clean:
	rm *.o read_imu
