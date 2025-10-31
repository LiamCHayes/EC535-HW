#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

size_t executed_instructions = 0;
size_t cycle_count = 0;
size_t n_hits_local_memory = 0;
size_t n_ld_st_instructions = 0;

int8_t R1;
int8_t R2;
int8_t R3;
int8_t R4;
int8_t R5;
int8_t R6;

int8_t local_memory[256];
int addresses_accessed[256] = {0};

int cmp_result;
int next_address = -1;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("[Error] expected ./myISS [Program Name]\n");
        return 1;
    }

    // Open file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("[Error] unable to open file %s\n", argv[1]);
        return 1;
    }

    // Parse code
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Get the line label and instruction
        char *line_label;
        char *instruction;
        line_label = strtok(line, "\t");
        instruction = strtok(NULL, " ");

        // Check if there is a jump to a different address
        if (next_address != -1) {
            int curr_address = atoi(line_label);
            
            if (curr_address > next_address) {
                fclose(file);
                FILE *file = fopen(argv[1], "r");
                if (file == NULL) {
                    printf("[Error] unable to open file %s\n", argv[1]);
                    return 1;
                }
                continue;
            } else if (curr_address < next_address) {
                continue;
            } else {
                next_address = -1;
            }
        }

        // Go through the instruction and decide what to do
        if (strcmp(instruction, "MOV") == 0) {
            // Move the second argument into the first argument buffer
            executed_instructions += 1;
            cycle_count += 1;

            char *buffer;
            char *value_char;

            buffer = strtok(NULL, " ");
            value_char = strtok(NULL, "\0");
            int buffer_num = (int)buffer[1] - '0';
            int value = atoi(value_char);

            switch (buffer_num) {
                case (1):
                    R1 = value;
                    break;
                case (2):
                    R2 = value;
                    break;
                case (3):
                    R3 = value;
                    break;
                case (4):
                    R4 = value;
                    break;
                case (5):
                    R5 = value;
                    break;
                case (6):
                    R6 = value;
                    break;
            }
        } else if (strcmp(instruction, "ADD") == 0) {
            // Add number into the first argument buffer
            executed_instructions += 1;
            cycle_count += 1;

            char *buffer1;
            char *arg2;
            int contents;

            buffer1 = strtok(NULL, " ");
            int buffer1_num = (int)buffer1[1] - '0';

            arg2 = strtok(NULL, "\0");
            if (arg2[0] == 'R') {
                // Extract value in buffer2
                int buffer2_num = (int)arg2[1] - '0';

                switch (buffer2_num) {
                    case (1):
                        contents = R1;
                        break;
                    case (2):
                        contents = R2;
                        break;
                    case (3):
                        contents = R3;
                        break;
                    case (4):
                        contents = R4;
                        break;
                    case (5):
                        contents = R5;
                        break;
                    case (6):
                        contents = R6;
                        break;
                }
            } else {
                contents = atoi(arg2);
            }

            // Add to buffer1
            switch (buffer1_num) {
                case (1):
                    R1 += contents;
                    break;
                case (2):
                    R2 += contents;
                    break;
                case (3):
                    R3 += contents;
                    break;
                case (4):
                    R4 += contents;
                    break;
                case (5):
                    R5 += contents;
                    break;
                case (6):
                    R6 += contents;
                    break;
            }
        } else if (strcmp(instruction, "CMP") == 0) {
            // Compare the contents of the two buffers
            executed_instructions += 1;
            cycle_count += 1;

            char *buffer1;
            char *buffer2;

            buffer1 = strtok(NULL, " ");
            buffer2 = strtok(NULL, "\0");
            int buffer1_num = (int)buffer1[1] - '0';
            int buffer2_num = (int)buffer2[1] - '0';

            int contents1;
            switch (buffer1_num) {
                case (1):
                    contents1 = R1;
                    break;
                case (2):
                    contents1 = R2;
                    break;
                case (3):
                    contents1 = R3;
                    break;
                case (4):
                    contents1 = R4;
                    break;
                case (5):
                    contents1 = R5;
                    break;
                case (6):
                    contents1 = R6;
                    break;
            }

            int contents2;
            switch (buffer2_num) {
                case (1):
                    contents2 = R1;
                    break;
                case (2):
                    contents2 = R2;
                    break;
                case (3):
                    contents2 = R3;
                    break;
                case (4):
                    contents2 = R4;
                    break;
                case (5):
                    contents2 = R5;
                    break;
                case (6):
                    contents2 = R6;
                    break;
            }

            if (contents1 == contents2) {
                cmp_result = 1;
            } else {
                cmp_result = 0;
            }
        } else if (strcmp(instruction, "JE") == 0) {
            // Jump to the instruction number if the last compare was equal
            executed_instructions += 1;
            cycle_count += 1;

            char *value_char;

            value_char = strtok(NULL, "\0");
            int je_address = atoi(value_char);

            if (cmp_result == 1) {
                // Jump to the instruction at the address
                next_address = je_address;
                cmp_result = 0;
            }
        } else if (strcmp(instruction, "JMP") == 0) {
            // Jump to the instruction at the address
            executed_instructions += 1;
            cycle_count += 1;

            char *value_char;

            value_char = strtok(NULL, "\0");
            next_address = atoi(value_char);
        } else if (strcmp(instruction, "LD") == 0) {
            // Store contents of the address in buffer2 in buffer1
            n_ld_st_instructions += 1;
            executed_instructions += 1;

            char *buffer1;
            char *buffer2;

            buffer1 = strtok(NULL, " ");
            buffer2 = strtok(NULL, "\0");
            int buffer1_num = (int)buffer1[1] - '0';
            int buffer2_num = (int)buffer2[2] - '0';

            // Get address from buffer2
            int address;
            switch (buffer2_num) {
                case (1):
                    address = R1;
                    break;
                case (2):
                    address = R2;
                    break;
                case (3):
                    address = R3;
                    break;
                case (4):
                    address = R4;
                    break;
                case (5):
                    address = R5;
                    break;
                case (6):
                    address = R6;
                    break;
            }

            // load contents from address
            int contents;
            contents = local_memory[address];

            // check if this memory has been accessed
            int in_local_memory = 0;
            for (int i=0; i < 256; i++) {
                int accesed = addresses_accessed[i];
                if (accesed == address) {
                    // We accessed local memory
                    in_local_memory = 1;
                    break;
                }
                if (accesed == 0) {
                    // We exhaused the list of local memory we have accessed
                    addresses_accessed[i] = address;
                    break;
                }
            }

            // Track number of clock cycles the memory access took
            if (in_local_memory == 0) {
                cycle_count += 50;
            } else {
                cycle_count += 2;
                n_hits_local_memory += 1;
            }

            // store contents in buffer1
            switch (buffer1_num) {
                case (1):
                    R1 = contents;
                    break;
                case (2):
                    R2 = contents;
                    break;
                case (3):
                    R3 = contents;
                    break;
                case (4):
                    R4 = contents;
                    break;
                case (5):
                    R5 = contents;
                    break;
                case (6):
                    R6 = contents;
                    break;
            }
        } else if (strcmp(instruction, "ST") == 0) {
            // Store contents of second argument into the first argument address in the local memory
            n_ld_st_instructions += 1;
            executed_instructions += 1;

            char *buffer1;
            char *buffer2;

            buffer1 = strtok(NULL, " ");
            buffer2 = strtok(NULL, "\0");
            int buffer1_num = (int)buffer1[2] - '0';
            int buffer2_num = (int)buffer2[1] - '0';

            int address;
            switch (buffer1_num) {
                case (1):
                    address = R1;
                    break;
                case (2):
                    address = R2;
                    break;
                case (3):
                    address = R3;
                    break;
                case (4):
                    address = R4;
                    break;
                case (5):
                    address = R5;
                    break;
                case (6):
                    address = R6;
                    break;
            }

            int contents;
            switch (buffer2_num) {
                case (1):
                    contents = R1;
                    break;
                case (2):
                    contents = R2;
                    break;
                case (3):
                    contents = R3;
                    break;
                case (4):
                    contents = R4;
                    break;
                case (5):
                    contents = R5;
                    break;
                case (6):
                    contents = R6;
                    break;
            }

            local_memory[address] = contents;

            // check if this memory has been accessed
            int in_local_memory = 0;
            for (int i=0; i < 256; i++) {
                int accesed = addresses_accessed[i];
                if (accesed == address) {
                    // We accessed local memory
                    in_local_memory = 1;
                    break;
                }
                if (accesed == 0) {
                    // We exhaused the list of local memory we have accessed
                    addresses_accessed[i] = address;
                    break;
                }
            }

            // Track number of clock cycles the memory access took
            if (in_local_memory == 0) {
                cycle_count += 50;
            } else {
                cycle_count += 2;
                n_hits_local_memory += 1;
            }

        } else {
            printf("Unknown instruction: %s\n", instruction);
            return 1;
        }
    }

    fclose(file);

    printf("Total number of executed instructions: %ld\n", executed_instructions);
    printf("Total number of clock cycles: %ld\n", cycle_count);
    printf("Number of hits to local memory: %ld\n", n_hits_local_memory);
    printf("Total number of executed LD/ST instructions: %ld\n", n_ld_st_instructions);
    return 0;
}
