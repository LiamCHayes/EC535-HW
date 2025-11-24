#include "encoder_driver.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


double get_encoder_rot(int analog_pin) {
	//** returns encoder value in radians **//

    // The analog input devices are located in this directory
    // TODO make fp an input to this function and open fp in main.c
    const char *adc_path = "/sys/bus/iio/devices/iio:device0/in_voltage";
    FILE *fp;
    char buffer[20];
    int analog_value;

        // Open the device file for the analog pin (e.g., in_voltage0)
        // the pin you want to read is AIN (0-7)
        char file_path[100];
        sprintf(file_path, "%s%d_raw", adc_path, analog_pin);
        fp = fopen(file_path, "r");

        if (fp == NULL) {
            perror("Error opening file\n");
            return 1;
        }


        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            // Convert the string to an integer
            analog_value = atoi(buffer);
            //printf("Analog value: %d\n", analog_value);
        }

	//3760 is ~ 1.65V (max reading from analog), so 3760 is 0/2pi
	double encoder_position = (double)analog_value / 3760 * 2 * 3.14159265;

        fclose(fp);
        usleep(100);

        return encoder_position;
}
