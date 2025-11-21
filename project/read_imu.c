#include <stdio.h>
#include <stdint.h>
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

    // Make sure imu can send non0 value
    char reg_address = 0x0F;
    char data_buffer = {0};

    // Structure for I2C message transaction
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    // Message 1: Write the register address we want to read from
    messages[0].addr  = addr; // Slave address 0x68
    messages[0].flags = 0;    // Flags = 0 for write
    messages[0].len   = 1;    // Length of message (1 byte: the register address)
    messages[0].buf   = &reg_address; // Pointer to the register address variable

    // Message 2: Read 1 byte of data back
    messages[1].addr  = addr;
    messages[1].flags = I2C_M_RD; // Flags = I2C_M_RD for read
    messages[1].len   = 1;        // Length of message (1 byte: the response)
    messages[1].buf   = &data_buffer; // Pointer to where we store the response

    // Set up the packets structure
    packets.msgs = messages;
    packets.nmsgs = 2; // We have two messages in this transaction

    // Perform the combined I2C transaction
    if (ioctl(file, I2C_RDWR, &packets) < 0) {
        perror("Failed to execute I2C_RDWR command");
        exit(1);
    }

    printf("Who Am I register value: 0x%X\n", data_buffer);

	char config[2] = {0};
	config[0] = 0x6B;
	config[1] = 0x00;
	if (write(file, config, 2) != 2) {
	    perror("Failed to write to the i2c bus #1");
	    exit(1);
	}

    for (int i=0; i < 10; i++) {
        char reg = 0x3B; // Accelerometer X-axis high byte register
        if (write(file, &reg, 1) != 1) {
            perror("Failed to write to the i2c bus");
            exit(1);
        }

        unsigned char data[6] = {1}; // For X, Y, Z accelerometer data (high
        if (read(file, data, 6) != 6) {
            perror("Failed to read from the i2c bus");
            exit(1);
        }

        // Process data (e.g., combine high and low bytes, convert to
        int16_t accelX = (data[0] << 8) | data[1];
        int16_t accelY = (data[2] << 8) | data[3];
        int16_t accelZ = (data[4] << 8) | data[5];

        printf("Acceleration X: %d, Y: %d, Z: %d\n", accelX, accelY, accelZ);
    }

	close(file);
    return 0;
}
