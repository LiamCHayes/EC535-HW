#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdint.h>

#define I2C_BUS_PATH "/dev/i2c-2"
#define AS5600_ADDR  0x36
#define CONF_REG_ADDR 0x07
#define ANALOG_CONF_VALUE 0x0000 // 16-bit value for Analog Output (Proportional)

int configure_as5600_analog() {
    int file;
    uint8_t write_buffer[3];
    int status;

    //Open the I2C Bus Device File
    if ((file = open(I2C_BUS_PATH, O_RDWR)) < 0) {
        perror("Failed to open the I2C bus file");
        return -1;
    }

    //Set the Slave Address
    if (ioctl(file, I2C_SLAVE, AS5600_ADDR) < 0) {
        perror("Failed to set I2C slave address");
        close(file);
        return -1;
    }

    // Write the Configuration Value to the CONF Register (0x07)
    
    // The AS5600 uses an auto-incrementing pointer for multi-byte writes.
    // The CONF register (0x07) is 16-bits long.
    // The buffer structure is: [Register Address, MSB of value, LSB of value]

    // Register Address (High Byte of the address pointer)
    write_buffer[0] = CONF_REG_ADDR; 
    
    // MSB (Most Significant Byte) of the 16-bit value 0x0000
    write_buffer[1] = (uint8_t)((ANALOG_CONF_VALUE >> 8) & 0xFF); 
    
    // LSB (Least Significant Byte) of the 16-bit value 0x0000
    write_buffer[2] = (uint8_t)(ANALOG_CONF_VALUE & 0xFF); 

    // Write all 3 bytes at once
    status = write(file, write_buffer, 3);
    
    if (status != 3) {
        fprintf(stderr, "Failed to write all 3 bytes to the AS5600. Status: %d\n", status);
        close(file);
        return -1;
    }

    printf("AS5600 configured successfully to Analog Output (Voltage Proportional) mode.\n");
    close(file);
    return 0;
}

int main() {
    configure_as5600_analog();
    return 0;
}
