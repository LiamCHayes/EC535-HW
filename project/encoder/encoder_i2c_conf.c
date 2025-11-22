#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>

#define I2C_BUS_PATH      "/dev/i2c-2"
#define AS5600_ADDR       0x36
#define CONF_REG_ADDR     0x07
#define BURN_REG_ADDR     0x0C
#define ANALOG_CONF_VALUE 0x0000 // 16-bit value for Analog Output (Proportional)
#define BURN_COMMAND      0x80

// Function to perform the configuration write
int write_config(int file) {
    uint8_t write_buffer[3];
    int status;

    // Set the register address (0x07)
    write_buffer[0] = CONF_REG_ADDR; 
    
    // Set the 16-bit configuration value 0x0000 (MSB then LSB)
    write_buffer[1] = (uint8_t)((ANALOG_CONF_VALUE >> 8) & 0xFF); 
    write_buffer[2] = (uint8_t)(ANALOG_CONF_VALUE & 0xFF); 

    status = write(file, write_buffer, 3);
    
    if (status != 3) {
        fprintf(stderr, "Failed to write configuration to AS5600. Status: %d\n", status);
        return -1;
    }
    printf("AS5600 configured successfully to Analog Output mode.\n");
    return 0;
}

// Function to perform the NVM burn
int burn_settings(int file) {
    uint8_t write_buffer[2];
    int status;

    // The buffer structure is: [BURN Register Address, Burn Command]
    write_buffer[0] = BURN_REG_ADDR; 
    write_buffer[1] = BURN_COMMAND; 

    // Write the 2 bytes (Address + Command)
    status = write(file, write_buffer, 2);
    
    if (status != 2) {
        fprintf(stderr, "Failed to burn settings to NVM. Status: %d\n", status);
        return -1;
    }

    printf("Settings successfully burned to Non-Volatile Memory (NVM).\n");
    return 0;
}

int configure_and_burn() {
    int file;
    
    // 1. Open I2C Bus and Set Slave Address
    if ((file = open(I2C_BUS_PATH, O_RDWR)) < 0) {
        perror("Failed to open the I2C bus file");
        return -1;
    }
    if (ioctl(file, I2C_SLAVE, AS5600_ADDR) < 0) {
        perror("Failed to set I2C slave address");
        close(file);
        return -1;
    }

    // 2. Write the Volatile Configuration
    if (write_config(file) != 0) {
        close(file);
        return -1;
    }
    
    // Wait a short period (Datasheet doesn't specify, but a tiny delay is good practice)
    usleep(1000); 

    // 3. Burn the Settings to NVM
    if (burn_settings(file) != 0) {
        close(file);
        return -1;
    }
    
    close(file);
    return 0;
}

int main() {
    printf("Starting AS5600 Configuration and NVM Burn...\n");
    configure_and_burn();
    return 0;
}
