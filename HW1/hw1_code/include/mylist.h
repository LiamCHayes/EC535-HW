#ifndef MYLIST_H
#define MYLIST_H

#include <stdio.h>

typedef struct Node {
    unsigned int data;
    unsigned int decimal_mirror;
    unsigned int num_sequences;
    struct Node* next;
} Node;

Node* CreateNode(
        unsigned int,
        unsigned int,
        unsigned int
        );
Node* InsertNode(Node*, Node*);
Node* PopToFile(Node*, FILE*);

#endif
