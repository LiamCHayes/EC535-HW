CC = arm-linux-gnueabihf-gcc
CFLAGS = -std=c99 -Wall -I. -static-libgcc -O0 -lm
DEPS = 
OBJ = qsort_large.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

qsort_large: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	
clean:
	rm *.o qsort_large output*
