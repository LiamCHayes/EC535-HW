#ifndef ENCODER_DRIVER_H
#define ENCODER_DRIVER_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//takes the analog pin (ie: AIN0) as input
//outputs the analog positional encoder value at respective pin in radians
double get_encoder_rot(int analog_pin);

#endif
