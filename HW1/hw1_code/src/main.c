/* Liam Hayes - liamh */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "bits.h"
#include "mylist.h"

int main(int argc, char *argv[]) {
    /* Parse arguments */
    if (argc != 3) {
        printf("Incorrect number of arguments. Please use: %s input_file.txt output_file.txt\n", argv[0]);
        perror("Cannot parse arguments");
        return 1;
    }

    char *input_filepath = argv[1];
    char *output_filepath = argv[2];

    /* Open files */
    FILE *input_file;
    FILE *output_file;

    input_file = fopen(input_filepath, "r");
    output_file = fopen(output_filepath, "w");

    if (!input_file) {
        perror("Failed to open input file");
        return 1;
    }
    
    if (!output_file) {
        perror("Failed to open output file");
        return 1;
    }

    /* Compute binary mirror and 010 sequences for each line in the file */
    char line[32];
    Node* first_node = NULL;
    while (fgets(line, sizeof(line), input_file)) {
        /* Process line to convert to unsigned int */
        line[strcspn(line, "\n")] = '\0';

        /* Convert line to unsigned int */
        char *endptr;
        unsigned long decimal_long = strtoul(line, &endptr, 10);

        if (*endptr != '\0') {
            perror("Cannot parse integer");
            return 1;
        }

        if (decimal_long > UINT_MAX) {
            perror("Integer is too large to cast to unsigned int");
            return 1;
        }

        unsigned int decimal = (unsigned int) decimal_long;

        /* Compute binary mirror */
        unsigned int decimal_mirror = BinaryMirror(decimal);

        /* Count 010 sequences */
        unsigned int num_sequences = CountSequence(decimal);

        /* Add to linked list */
        Node* new_node = CreateNode(decimal, decimal_mirror, num_sequences);
        if (first_node == NULL) {
            first_node = new_node;
        }
        first_node = InsertNode(first_node, new_node);
    }

    /* Write to file from linked list */
    do {
        first_node = PopToFile(first_node, output_file);
    } while (first_node->next != NULL);

    fclose(input_file);
    fclose(output_file);

    return 0;
}
