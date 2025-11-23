#include <imu_driver.h>
#include <encoder_driver.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define I2C_BUS_FILE "/dev/i2c-2"
#define ENCODER_ANALOG_PIN 0

int main() {
    // Setup IMU
    int imu_file_handle;

    if ((imu_file_handle = open(I2C_BUS_FILE, O_RDWR)) < 0) {
        perror("Failed to open the i2c bus");
        exit(1);
    }

    int success = imu_init(imu_file_handle);
    if (success != 0) {
        perror("Could not initialize IMU device");
        exit(1);
    }

    // Loop and get data
    for (int i=0; i<10000; i++) {
        imu_data_t imu_data = imu_read(imu_file_handle);
        double encoder_postion = get_encoder_rot(ENCODER_ANALOG_PIN);
    }

    return 0;
}
