#include "mylist.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

Node* CreateNode(
        unsigned int data,
        unsigned int decimal_mirror,
        unsigned int num_sequences) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->decimal_mirror = decimal_mirror;
    node->num_sequences = num_sequences;
    node->next = NULL;
    return node;
}

/* Gets ascii representation */
static int* get_ascii(unsigned int data) {
    char str[32];
    sprintf(str, "%u", data);

    int* ascii_values = malloc((strlen(str)+1) * sizeof(int));
    for (size_t i=0; i < strlen(str); i++) {
        ascii_values[i] = (unsigned char)str[i];
    }
    ascii_values[strlen(str)] = 0;

    return ascii_values;
}

/* Returns true if the first node is greater based on ascii representation */
static bool greater_than_ascii(Node* node1, Node* node2) {
    int* ascii1 = get_ascii(node1->data);
    int* ascii2 = get_ascii(node2->data);

    for (int i=0; ascii1!=0 && ascii2!=0; i++) {
        if (ascii1[i] > ascii2[i]) return true;
        if (ascii2[i] > ascii1[i]) return false;
    }

    /* Return true if they are exactly the same since it doesn't matter */
    return true;
}

Node* InsertNode(Node* first_node, Node* new_node) {
    if (first_node == new_node) {
        return first_node;
    } else if (greater_than_ascii(first_node, new_node)) {
        new_node->next = first_node;
        return new_node;
    }

    /* Traverse the list and find the first node that's greater than new_node */
    Node* current_node = first_node;
    Node* next_node = first_node->next;

    while (next_node != NULL) {
        if (greater_than_ascii(new_node, current_node) && greater_than_ascii(next_node, new_node)) {
            current_node->next = new_node;
            new_node->next = next_node;
            return first_node;
        } else {
            current_node = next_node;
            next_node = current_node->next;
        }
    }

    current_node->next = new_node;
    return first_node;
}

Node* PopToFile(Node* first_node, FILE* output_file) {
    fprintf(output_file, "%-10u %u\n", first_node->decimal_mirror, first_node->num_sequences);
    return first_node->next;
}
