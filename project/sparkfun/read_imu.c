#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdint.h>

#define I2C_BUS_FILE "/dev/i2c-2"
#define DEVICE_ADDRESS 0x68
#define REG_BANK_SEL 0x7F
#define PWR_MGMT_1 0x06
#define PWR_MGMT_2 0x07
#define WHO_AM_I_REG 0x00
#define WHO_AM_I_EXPECTED 0xEA

#define ACCEL_XOUT_H 0x2D

int main() {
    int file_handle;

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

    // Set the current bank to bank 0
    uint8_t bank_select_data[2] = {REG_BANK_SEL, 0x00};
    if (write(file_handle, bank_select_data, 2) != 2) {
        perror("Failed to select User Bank 0 for reset");
        // Continue, as this might not be fatal yet
    }

    // Soft reset to default values to make sure the chip is in a clean state
    uint8_t reset_cmd[2] = {PWR_MGMT_1, 0x80};
    if (write(file_handle, reset_cmd, 2) != 2) {
        perror("Failed to perform soft reset");
    }

    usleep(100000);

    // Check Who Am I
    printf("Who Am I test...\n");
    uint8_t reg_addr = WHO_AM_I_REG;
    uint8_t whoami_data = 0;
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data msgset;

    msgs[0].addr = DEVICE_ADDRESS;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &reg_addr;
    
    msgs[1].addr = DEVICE_ADDRESS;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 1;
    msgs[1].buf = &whoami_data;

    msgset.msgs = msgs;
    msgset.nmsgs = 2;

    if (ioctl(file_handle, I2C_RDWR, &msgset) < 0) {
        perror("Failed to perform Who am I test");
        close(file_handle);
        exit(1);
    }

    if (whoami_data == WHO_AM_I_EXPECTED) {
        printf("Who am I test passed!\n");
    } else {
        printf("Who am I test failed\n");
    }

    // Wake up chip and select PLL clock
    uint8_t clock_select_cmd[2] = {PWR_MGMT_1, 0x01};
    if (write(file_handle, clock_select_cmd, 2) != 2) {
        perror("Failed to set PLL clock");
    }

    // Enable accelerometer and gyroscope
    uint8_t enable_sensors_cmd[2] = {PWR_MGMT_2, 0x00};
    if (write(file_handle, enable_sensors_cmd, 2) != 2) {
        perror("Failed to enable sensors");
    }

    usleep(10000);

    // Loop and read sensor values
    for (int i=0; i < 10000; i++) {
        // Acceleration
        uint8_t reg_addr = ACCEL_XOUT_H;
        uint8_t accel_data_buffer[2] = {0, 0};
        struct i2c_msg msgs[2];
        struct i2c_rdwr_ioctl_data msgset;

        msgs[0].addr = DEVICE_ADDRESS;
        msgs[0].flags = 0;
        msgs[0].len = 1;
        msgs[0].buf = &reg_addr;

        msgs[1].addr = DEVICE_ADDRESS;
        msgs[1].flags = I2C_M_RD;
        msgs[1].len = 2;
        msgs[1].buf = &accel_data_buffer;

        msgset.msgs = msgs;
        msgset.nmsgs = 2;

        if (ioctl(file_handle, I2C_RDWR, &msgset) < 0) {
            perror("Failed to read Accel X data");
            close(file_handle);
            exit(1);
        }

        int16_t accel_x = (int16_t)(accel_data_buffer[0] << 8 | accel_data_buffer[1]);
        printf("Raw acceleration x: %d\n", accel_x);

        // Gyroscope
    }

    // Close the file handle
    close(file_handle);

    return 0;
}

