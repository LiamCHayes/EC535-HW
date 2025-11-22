#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // The analog input devices are located in this directory
    const char *adc_path = "/sys/bus/iio/devices/iio:device0/in_voltage";
    FILE *fp;
    char buffer[20];
    int analog_value;

    for (int i=0; i<10; i++) {
        // Open the device file for the analog pin (e.g., in_voltage0)
        // "0" is the pin you want to read (0-7)
        char file_path[100];
        sprintf(file_path, "%s%d_raw", adc_path, 0);
        fp = fopen(file_path, "r");

        if (fp == NULL) {
            perror("Error opening file: %s\n", file_path);
            return 1;
        }


        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            // Convert the string to an integer
            analog_value = atoi(buffer);
            printf("Analog value: %d\n", analog_value);
        }

        fclose(fp);

        usleep(100000); // Sleep for 100ms
    }

    return 0;
}
