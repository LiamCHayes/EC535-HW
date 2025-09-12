/* Liam Hayes - liamh */

#include "bits.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

unsigned int BinaryMirror(unsigned int dec) {
    /* Allocate space for the binary representation */
    char* binary = malloc(33);
    if (!binary) {
        perror("malloc failed");
        return NULL;
    }

    /* Read the bits with bitwise operators from least significant to most significant */
    for (int i=0; i<32; i++) {
        binary[i] = ((dec >> i) & 1) ? '1': '0';
    }
    binary[32] = '\0';

    /* Convert the mirror to an unsigned int */
    unsigned int result = 0;
    for (int i = 0; i < 32; i++) {
        result <<= 1;
        if (binary[i] == '1') {
            result |= 1;
        }
    }

    return result;
}

unsigned int CountSequence(unsigned int dec) {
    /* Allocate space for the binary representation */
    char* binary = malloc(33);
    if (!binary) {
        perror("malloc failed");
        return NULL;
    }

    /* Read the bits with bitwise operators from most significant to least significant */
    for (int i=31; i>=0; i--) {
        binary[i] = ((dec >> i) & 1) ? '1': '0';
    }
    binary[32] = '\0';

    /* Count the number of 010 sequences */
    unsigned int counter = 0;
    for (int i=0; i<29; i++) {
        if (binary[i] == '0' && binary[i+1] == '1' && binary[i+2] == '0') {
            counter++;
        }
    }
    return counter;
}

