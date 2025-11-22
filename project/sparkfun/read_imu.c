#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>

#define I2C_BUS_FILE "/dev/i2c-2"
#define DEVICE_ADDRESS 0x68
#define WHO_AM_I_REG 0x00
#define WHO_AM_I_EXPECTED 0xEA

int main() {
    int file_handle;
    unsigned char buffer[2];
    unsigned char data_addr_buffer[1];

    // Open the I2C bus file
    if ((file_handle = open(I2C_BUS_FILE, O_RDWR)) < 0) {
        perror("Failed to open the i2c bus");
        exit(1);
    }

    // Set the I2C slave address for the next transfers
    if (ioctl(file_handle, I2C_SLAVE, DEVICE_ADDRESS) < 0) {
        perror("Failed to acquire bus access and/or talk to slave");
        close(file_handle);
        exit(1);
    }

    // Check Who Am I

    // Close the file handle
    close(file_handle);

    return 0;
}

