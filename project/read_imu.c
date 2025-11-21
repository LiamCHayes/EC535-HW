#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <string.h>

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

    // --- WHO AM I Test using i2c_rdwr ioctl ---

    uint8_t reg_address = 0x0F; // The register we want to read (Who Am I)
    uint8_t data_buffer = 0;    // Buffer to hold the response

    // Declare the message structures as an array of 2
    struct i2c_msg messages[2]; 

    // Declare the overall transaction structure
    struct i2c_rdwr_ioctl_data packets;

    // --- Message 0: Write the register address (0x0F) ---
    messages[0].addr  = addr;       // Slave address 0x68
    messages[0].flags = 0;          // Flags = 0 for write
    messages[0].len   = 1;          // Length of message (1 byte: the register address)
    messages[0].buf   = &reg_address; // Pointer to the register address variable

    // --- Message 1: Read 1 byte of data back ---
    messages[1].addr  = addr;       // Slave address 0x68
    messages[1].flags = I2C_M_RD;   // Flags = I2C_M_RD for read
    messages[1].len   = 1;          // Length of message (1 byte: the response)
    messages[1].buf   = &data_buffer; // Pointer to where we store the response

    // Set up the packets structure
    packets.msgs = messages; // Point to the start of the messages array
    packets.nmsgs = 2;       // We have two messages in this transaction

    // Perform the combined I2C transaction using IOCTL
    if (ioctl(file, I2C_RDWR, &packets) < 0) {
        perror("Failed to execute I2C_RDWR command");
        exit(1);
    }

    printf("Who Am I register value: 0x%X\n", data_buffer);
    
    close(file);
    return 0;
}

