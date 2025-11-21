#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

int main() {
    // Open i2c file
	int file;
	char *bus = "/dev/i2c-2";
	if ((file = open(bus, O_RDWR)) < 0) {
	    perror("Failed to open the i2c bus");
	    exit(1);
	}

    // Configure the bus to communicate with the IMU
	int addr = 0x68;
	if (ioctl(file, I2C_SLAVE, addr) < 0) {
	    perror("Failed to acquire bus access and/or talk to slave");
	    exit(1);
	}

	char config[2] = {0};
	config[0] = 0x6B;
	config[1] = 0x00;
	if (write(file, config, 2) != 2) {
	    perror("Failed to write to the i2c bus #1");
	    exit(1);
	}

	char reg = 0x3B; // Accelerometer X-axis high byte register
	if (write(file, &reg, 1) != 1) {
	    perror("Failed to write to the i2c bus");
	    exit(1);
	}

    for (int i=0; i < 10000; i++) {
        char data[6] = {1}; // For X, Y, Z accelerometer data (high
        if (read(file, data, 6) != 6) {
            perror("Failed to read from the i2c bus");
            exit(1);
        }

        // Process data (e.g., combine high and low bytes, convert to
        int accelX = (data[0] << 8) | data[1];

        printf("Acceleration X: %d\n", accelX);
    }

	close(file);
}
