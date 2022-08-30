#include "node.h"
#include <stdio.h>
#include <stdlib.h>

Node *node_create(uint8_t symbol, uint64_t frequency) {
    /*creates a node using malloc with allocates size of node
     * sets symbol and frequency to parameter value
     * sets left node and right node to null
     * finally returns node*/
    Node *node = malloc(sizeof(Node));
    node->symbol = symbol;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void node_delete(Node **n) {
    /*deletes node using free
     * sets node to null*/
    free(*n);
    *n = NULL;
    return;
}

Node *node_join(Node *left, Node *right) {
    uint64_t sum = left->frequency + right->frequency; //sums frequencies
    Node *parent = node_create('$', sum); //creates parent
    parent->left = left; //sets left and right of parent
    parent->right = right;
    return parent; //returns parent
}

void node_print(Node *n) {
    printf("(%d, %ld)\n", n->symbol, n->frequency); //prints node
}
