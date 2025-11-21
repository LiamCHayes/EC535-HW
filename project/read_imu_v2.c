#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <stdint.h>

#define I2C_BUS_FILE "/dev/i2c-2"
#define DEVICE_ADDRESS 0x6B
#define REGISTER_ADDRESS 0x18
#define WHO_AM_I_REG 0x0F

// Setup addresses
#define CTRL3_C 0x12
#define CTRL9_XL 0x18
#define CTRL1_XL 0x10
#define CTRL10_C 0x19
#define CTRL2_G 0x11
#define INT1_CTRL 0x0D

// Gettind data addresses
#define STATUS_REG 0x1E
#define OUTX_L_XL 0x28
#define OUTX_H_XL 0x29
#define OUTY_L_XL 0x2A
#define OUTY_H_XL 0x2B
#define OUTZ_L_XL 0x2C
#define OUTZ_H_XL 0x2D

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

    // Who am i test
    // uint8_t config[2] = {WHO_AM_I_REG, 0x00};
    // if (write(file_handle, config, 1) != 1) {
            // perror("Failed to write to the i2c device");
            // exit(1);
    // }
// 
    // uint8_t value;
    // if (read(file_handle, &value, 1) != 1) {
            // perror("Failed to read from the i2c device");
            // exit(1);
    // } else {
        // printf("Value read from register 0x%02X at address 0x%02X: 0x%02X\n", WHO_AM_I_REG, REGISTER_ADDRESS, value);
    // }

    printf("[INFO] Initializing...\n");

    // Enable block data update and auto-increment
    buffer[0] = CTRL3_C;
    buffer[1] = 0x44;
    if (write(file_handle, buffer, 2) != 2) {
        perror("Failed to enable block data update");
        close(file_handle);
        exit(1);
    }

    // Enable acceleration axes
    buffer[0] = CTRL9_XL;
    buffer[1] = 0x38;
    if (write(file_handle, buffer, 2) != 2) {
        perror("Failed to enable acceleration axes");
        close(file_handle);
        exit(1);
    }
    
    // High performance mode
    buffer[0] = CTRL1_XL;
    buffer[1] = 0x60;
    if (write(file_handle, buffer, 2) != 2) {
        perror("Failed to enable high performance mode accelerometer");
        close(file_handle);
        exit(1);
    }

    // Set data ready interrupt for accelerometer
    buffer[0] = INT1_CTRL;
    buffer[1] = 0x01;
    if (write(file_handle, buffer, 2) != 2) {
        perror("Failed to enable accelerometer data ready interrupt");
        close(file_handle);
        exit(1);
    }

    // Enable gyroscope
    // buffer[0] = CTRL10_C;
    // buffer[1] = 0x38;
    // if (write(file_handle, buffer, 2) != 2) {
        // perror("Failed to enable acceleration axes");
        // close(file_handle);
        // exit(1);
    // }
    // 
    // // High performance mode for gyroscope
    // buffer[0] = CTRL2_G;
    // buffer[1] = 0x60;
    // if (write(file_handle, buffer, 2) != 2) {
        // perror("Failed to enable high performance mode gyroscope");
        // close(file_handle);
        // exit(1);
    // }

    sleep(1);
    printf("[INFO] Initialized\n");

    // Read data for a certain amount of time
    for (int i=0; i < 1000; i++) {
        // Check if there is new data ready
        data_addr_buffer[0] = STATUS_REG;
        if (write(file_handle, data_addr_buffer, 1) != 1) {
            perror("Failed to write to status reg");
            close(file_handle);
            exit(1);
        }
        unsigned char status_data[1] = {0};
        if (read(file_handle, status_data, 1) != 1) {
            perror("Failed to read status reg");
            exit(1);
        }

        if ((status_data[0] & 0x01) == 0) {
            continue;
        }

        // Read all 6 acceleration data registers
        data_addr_buffer[0] = OUTX_L_XL | 0x80;
        if (write(file_handle, data_addr_buffer, 1) != 1) {
            perror("Failed to write to accelerometer starting address");
            close(file_handle);
            exit(1);
        }

        // Read the 6 bytes
        char acc_data[6] = {0};
        if (read(file_handle, acc_data, 6) != 6) {
            perror("Failed to read 6 acceleration data bytes");
            exit(1);
        }

        // Data combination and conversion
        int16_t accelX = (int16_t) ((acc_data[1] << 8) | acc_data[0]);
        int16_t accelY = (int16_t) ((acc_data[3] << 8) | acc_data[2]);
        int16_t accelZ = (int16_t) ((acc_data[5] << 8) | acc_data[4]);

        float sensitivity = 0.000061f;
        float accelX_g = accelX * sensitivity;
        float accelY_g = accelY * sensitivity;
        float accelZ_g = accelZ * sensitivity;

        printf("Acceleration raw: %d, Y: %d, Z: %d\n", accelX, accelY, accelZ);
        printf("Acceleration grams X: %.3f, Y: %.3f, Z: %.3f\n", accelX_g, accelY_g, accelZ_g);
    }

    // Close the file handle
    close(file_handle);

    return 0;
}

