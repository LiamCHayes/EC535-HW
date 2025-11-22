CC = arm-linux-gnueabihf-gcc
CFLAGS = -std=c99 -Wall -I. -static -O0
DEPS = 
OBJ = read_imu_v2.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

read_imu_v2: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	
clean:
	rm *.o read_imu_v2
