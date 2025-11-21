#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <errno.h>

#define I2C_BUS_FILE "/dev/i2c-2"
#define DEVICE_ADDRESS 0x6A
#define REGISTER_ADDRESS 0x28

int main() {
    int file_handle;
    char buffer[1];
    char result_buffer[1];

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

    // --- Read from a specific register (common pattern) ---
    // Some devices require you to write the register address first,
    // then read the data in a combined operation or separate operations.

    // 1. Specify the register address to read from
    for (int i=0; i < 1000; i++) {
        buffer[0] = REGISTER_ADDRESS;
        if (write(file_handle, buffer, 1) != 1) {
            perror("Failed to write register address");
            close(file_handle);
            exit(1);
        }

        char data[6] = {0}; // For X, Y, Z accelerometer data (high
        if (read(file_handle, data, 6) != 6) {
            perror("Failed to read from the i2c bus");
            exit(1);
        }

        // Process data (e.g., combine high and low bytes, convert to
        int accelX = (data[0] << 8) | data[1];
        int accelY = (data[2] << 8) | data[3];
        int accelZ = (data[4] << 8) | data[5];

        printf("Acceleration X: %d, Y: %d, Z: %d\n", accelX, accelY, accelZ);
    }

    // Close the file handle
    close(file_handle);

    return 0;
}

