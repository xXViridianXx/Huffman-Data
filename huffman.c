#include "huffman.h"
#include "io.h"
#include "stack.h"
#include <stddef.h>
#include "pq.h"
#include <stdio.h>

Node *build_tree(uint64_t hist[static ALPHABET]) {
    /*builds a tree using all the symbols present in the message
     * final thing in queue should be 1 node dequeue 2 highest priority
     * when I thing left, dequque it*/
    PriorityQueue *pq = pq_create(257); //create priority queue

    for (int i = 0; i < 256; i++) {
        if (hist[i] > 0) {
            Node *node = node_create(i, hist[i]); //creates node
            enqueue(pq, node); //enqueues to pq
        }
    }
    Node *parent, *left, *right, *root;
    parent = left = right = root = NULL;
    while (pq_size(pq) > 1) { //dequeue until pq has 1 element
        dequeue(pq, &left);
        dequeue(pq, &right);
        parent = node_join(left, right); //will propbably cause seg fault
        enqueue(pq, parent);
    }
    dequeue(pq, &root); //gets the final thing in stack
    pq_delete(&pq); //deletes pq
    return root;
}

static Code code = { 0 }; //init code
void build_codes(Node *root, Code table[static ALPHABET]) {
    /* builds the codes for each symbol by popping and pushing to bits
     * as traversing down tree. If leaf, save code to table of codes*/
    uint8_t popped = 0;
    ;
    if (root != NULL) {
        if (root->left == NULL && root->right == NULL) {
            table[root->symbol] = code; //storing in table of codes
            return;
        } else { //checks if root
            code_push_bit(&code, 0); //pushes 0 then moves left
            build_codes(root->left, table);
            code_pop_bit(&code, &popped); //pops the 0

            code_push_bit(&code, 1); //pushes 1 then moves right
            build_codes(root->right, table);
            code_pop_bit(&code, &popped); //pops 1
            return;
        }
    }
}

void dump_tree(int outfile, Node *root) {
    /*creates tree dump using L as leaf and I as parent node
     * Post traversal process meaning leaves checked then parent*/
    uint8_t buffer; //buffer to store 'L' 'I' and symbol
    if (root != NULL) {
        if (root->left == NULL && root->left == NULL) { //checks if leaf
            buffer = 'L'; // store symbol to buffer
            bytes_written += write_bytes(outfile, &buffer, sizeof(uint8_t)); //write 'L' to buffer
            buffer = root->symbol;
            bytes_written
                += write_bytes(outfile, &buffer, sizeof(uint8_t)); //write symbol to buffer
            return;
        }

        if (root->left != NULL || root->right != NULL) { //checks if parent
            dump_tree(outfile, root->left); //traverse left side of tree
            dump_tree(outfile, root->right); //traverse right of tree
            if (root->left != NULL || root->right != NULL) { //if parent
                buffer = 'I';
                bytes_written += write_bytes(outfile, &buffer, sizeof(uint8_t)); //write 'I'
                return;
            }
        }
    }
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    /*rebuild tree from output of dump_tree. If L, pop symbol push symbol
     * to stack. If I, pop first 2 from stack and join to create parent*/
    Stack *s = stack_create(257); //allocation
    Node *parent, *right, *left, *root;
    parent = right = left = root = NULL;
    uint32_t i = 0;
    while (i < nbytes) {
        if (tree[i] == 'L') {
            Node *node = node_create(tree[i + 1], 0); //create node of next thing
            stack_push(s, node); //pushing to stack
            i += 2;
        }
        if (tree[i] == 'I') { //checks if I
            stack_pop(s, &right); //right popped
            stack_pop(s, &left); // b/c pushed in that order
            parent = node_join(left, right); //create parent
            stack_push(s, parent); //push parent
            i += 1;
        }
    }
    stack_pop(s, &root); //gets the final thing in stack
    stack_delete(&s);
    return root;
}

void delete_tree(Node **root) {
    /*deletes leaf nodes first, then parents.
     * does this by traversing down tree*/
    if (root != NULL) { //checks is root exists
        if ((*root)->left == NULL && (*root)->right == NULL) { //if leaf, delete node
            node_delete(root);
            return;
        } else if ((*root)->left != NULL || (*root)->right != NULL) {
            //if ((*root)->left != NULL || (*root)->right != NULL) {
            delete_tree(&((*root)->left)); //traverse left
            delete_tree(&((*root)->right)); //traverse right
            node_delete(root); //delete parents
        }
    }
}
