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
    char buffer[2];
    char data_addr_buffer[1];

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

    // Acceleration data ready interrupt
    buffer[0] = INT1_CTRL;
    buffer[1] = 0x01;
    if (write(file_handle, buffer, 2) != 2) {
        perror("Failed to enable acceleration data ready interrupt accelerometer");
        close(file_handle);
        exit(1);
    }

    // Enable gyroscope
    buffer[0] = CTRL10_C;
    buffer[1] = 0x38;
    if (write(file_handle, buffer, 2) != 2) {
        perror("Failed to enable acceleration axes");
        close(file_handle);
        exit(1);
    }
    
    // High performance mode for gyroscope
    buffer[0] = CTRL2_G;
    buffer[1] = 0x60;
    if (write(file_handle, buffer, 2) != 2) {
        perror("Failed to enable high performance mode gyroscope");
        close(file_handle);
        exit(1);
    }

    // gyroscope data ready interrupt
    buffer[0] = INT1_CTRL;
    buffer[1] = 0x02;
    if (write(file_handle, buffer, 2) != 2) {
        perror("Failed to enable gyroscope data ready interrupt");
        close(file_handle);
        exit(1);
    }

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
        char status_data[1] = {0};
        if (read(file_handle, status_data, 1) != 1) {
            perror("Failed to read status reg");
            exit(1);
        }

        int first_bit = status_data[0] >> (CHAR_BIT - 1) & 1;
        printf("Data ready: %d\n", first_bit);
        if (!first_bit) {
            continue;
        }

        // Read accelerometer values
        // Accelerometer X
        data_addr_buffer[0] = OUTX_L_XL;
        if (write(file_handle, data_addr_buffer, 1) != 1) {
            perror("Failed to write to accelerometer x low data register");
            close(file_handle);
            exit(1);
        }
        char acc_x_l[1] = {0};
        if (read(file_handle, acc_x_l, 1) != 1) {
            perror("Failed to read accelerometer x low data");
            exit(1);
        }

        data_addr_buffer[0] = OUTX_H_XL;
        if (write(file_handle, data_addr_buffer, 1) != 1) {
            perror("Failed to write to accelerometer x high data register");
            close(file_handle);
            exit(1);
        }
        char acc_x_h[1] = {0};
        if (read(file_handle, acc_x_h, 1) != 1) {
            perror("Failed to read accelerometer x high data");
            exit(1);
        }

        // Accelerometer Y
        data_addr_buffer[0] = OUTY_L_XL;
        if (write(file_handle, data_addr_buffer, 1) != 1) {
            perror("Failed to write to accelerometer y low data register");
            close(file_handle);
            exit(1);
        }
        char acc_y_l[1] = {0};
        if (read(file_handle, acc_y_l, 1) != 1) {
            perror("Failed to read accelerometer y low data");
            exit(1);
        }

        data_addr_buffer[0] = OUTY_H_XL;
        if (write(file_handle, data_addr_buffer, 1) != 1) {
            perror("Failed to write to accelerometer y high data register");
            close(file_handle);
            exit(1);
        }
        char acc_y_h[1] = {0};
        if (read(file_handle, acc_y_h, 1) != 1) {
            perror("Failed to read accelerometer y high data");
            exit(1);
        }

        // Accelerometer Z
        data_addr_buffer[0] = OUTZ_L_XL;
        if (write(file_handle, data_addr_buffer, 1) != 1) {
            perror("Failed to write to accelerometer z low data register");
            close(file_handle);
            exit(1);
        }
        char acc_z_l[1] = {0};
        if (read(file_handle, acc_z_l, 1) != 1) {
            perror("Failed to read accelerometer z low data");
            exit(1);
        }

        data_addr_buffer[0] = OUTZ_H_XL;
        if (write(file_handle, data_addr_buffer, 1) != 1) {
            perror("Failed to write to accelerometer z high data register");
            close(file_handle);
            exit(1);
        }
        char acc_z_h[1] = {0};
        if (read(file_handle, acc_z_h, 1) != 1) {
            perror("Failed to read accelerometer z high data");
            exit(1);
        }
        
        // print buffers
        printf("Acceleration x low: %s\n", acc_x_l);
        printf("Acceleration x high: %s\n", acc_x_h);
        printf("Acceleration y low: %s\n", acc_y_l);
        printf("Acceleration y high: %s\n", acc_y_h);
        printf("Acceleration z low: %s\n", acc_z_l);
        printf("Acceleration z high: %s\n", acc_z_h);
        
        // int accelX = (data[0] << 8) | data[1];
        // int accelY = (data[2] << 8) | data[3];
        // int accelZ = (data[4] << 8) | data[5];
// 
        // printf("Acceleration X: %d, Y: %d, Z: %d\n", accelX, accelY, accelZ);
    }

    // Close the file handle
    close(file_handle);

    return 0;
}

