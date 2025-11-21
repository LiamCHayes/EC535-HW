#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define I2C_BUS_NUM 2 // Change to the bus you are using (e.g., 1 or 2)
#define LSM6DS33_ACCEL_ADDR 0x6A // Default I2C address for
#define LIS3MDL_MAG_ADDR 0x1E   // Default I2C address for magnetometer

// LSM6DS33 Registers
#define LSM6DS33_ACCEL_OUT_X_L 0x28
#define LSM6DS33_CTRL1_XL 0x10

// LIS3MDL Registers
#define LIS3MDL_OUT_X_L 0x28
#define LIS3MDL_CTRL_REG1 0x20

int open_i2c_bus(int bus_num) {
    char filename[20];
    snprintf(filename, 19, "/dev/i2c-%d", bus_num);
    int file_handle = open(filename, O_RDWR);
    if (file_handle < 0) {
        perror("Failed to open the I2C bus");
        exit(1);
    }
    return file_handle;
}

void write_register(int file_handle, int dev_addr, __u8 reg_addr, __u8 value) {
    if (ioctl(file_handle, I2C_SLAVE, dev_addr) < 0) {
        perror("Failed to acquire I2C bus access and/or talk to slave");
        exit(1);
    }
    i2c_smbus_write_byte_data(file_handle, reg_addr, value);
}

short read_word(int file_handle, int dev_addr, __u8 reg_addr) {
    if (ioctl(file_handle, I2C_SLAVE, dev_addr) < 0) {
        perror("Failed to acquire I2C bus access and/or talk to slave");
        exit(1);
    }
    int low_byte = i2c_smbus_read_byte_data(file_handle, reg_addr);
    int high_byte = i2c_smbus_read_byte_data(file_handle, reg_addr + 1);
    return (short)((high_byte << 8) | low_byte);
}

int main() {
    int file = open_i2c_bus(I2C_BUS_NUM);

    // Initialize Accelerometer/Gyro: Enable at 104 Hz ODR, +/-2g scale
    write_register(file, LSM6DS33_ACCEL_ADDR, LSM6DS33_CTRL1_XL,
0x30); // ODR 104Hz, +/-2g

    // Initialize Magnetometer: Enable at 10 Hz ODR, continuous conversion
    write_register(file, LIS3MDL_MAG_ADDR, LIS3MDL_CTRL_REG1, 0x50);

    while (1) {
        short ax = read_word(file, LSM6DS33_ACCEL_ADDR, LSM6DS33_ACCEL_OUT_X_L);
        short ay = read_word(file, LSM6DS33_ACCEL_ADDR,
LSM6DS33_ACCEL_OUT_X_L + 2);
        short az = read_word(file, LSM6DS33_ACCEL_ADDR,
LSM6DS33_ACCEL_OUT_X_L + 4);

        short mx = read_word(file, LIS3MDL_MAG_ADDR, LIS3MDL_OUT_X_L);
        short my = read_word(file, LIS3MDL_MAG_ADDR, LIS3MDL_OUT_X_L + 2);
        short mz = read_word(file, LIS3MDL_MAG_ADDR, LIS3MDL_OUT_X_L + 4);

        printf("Accel: X=%d Y=%d Z=%d | Mag: X=%d Y=%d Z=%d\n", ax,
ay, az, mx, my, mz);

        usleep(100000); // Read every 100ms
    }

    close(file);
    return 0;
}

