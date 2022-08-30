#include "pq.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct PriorityQueue { //PriorityQueue struct
    uint32_t tail;
    uint32_t head;
    uint32_t capacity;
    uint32_t current_size; //not needed, but whatever
    Node **Q;
};

PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q
        = (PriorityQueue *) malloc(sizeof(PriorityQueue)); //allocates memory to PriorityQueue q
    if (q) {
        q->tail = q->head = q->current_size = 0; //initialize
        q->capacity = capacity;
        q->Q = (Node **) calloc(q->capacity, sizeof(Node)); //creates Node array
        if (q->Q) { // checking if item is non-zero
            return q;
        }
        free(q); //if null free and return 0
        q = NULL;
    }
    return (PriorityQueue *) 0;
}

void pq_delete(PriorityQueue **q) {
    free((*q)->Q); //deletes Node array
    (*q)->Q = NULL;

    free(*q); //deletes priorityqueue
    *q = NULL;
}

bool pq_empty(PriorityQueue *q) { //checks is pq empty
    if (q) {

        return (pq_size(q) == 0); //empty if size == 0
    }
    return true;
}

bool pq_full(PriorityQueue *q) { //checks if pq full

    if (q) {
        return (q->head) == q->capacity; //full if size == capacity
    }
    return true;
}

uint32_t pq_size(PriorityQueue *q) { //returns current size of queue
    return q->current_size;
}

bool enqueue(PriorityQueue *q, Node *n) {
    if (q) {
        if (pq_full(q)) { //can't enqueue if queue full
            return false;
        }
        q->Q[q->head] = n; //set node to where index p->head is at
        q->current_size += 1; //increase current size by 1
        q->head += 1; //move head to next index
        return true; //return true if success
    }
    return false;
}

bool dequeue(PriorityQueue *q, Node **n) {

    if (q) {
        if (pq_empty(q)) {
            return false; //if empty, can't dequeue
        }
        Node *temp; //temp vaiable for swapping
        uint32_t lowest; //variable fro finding lowest
        lowest = q->tail; //always set lowest to thing to be removed

        /*loop finds lowest thing in array and keeps value
	 * if anything less than lowest element, it is now the new lowest
	 * since I always dequeue what tail is, swap lowest with where tail is*/
        for (uint32_t i = q->tail; i < q->head; i++) {
            if ((q->Q[i])->frequency < (q->Q[lowest])->frequency) {
                lowest = i;
            }
        }

        temp = q->Q[q->tail];
        q->Q[q->tail] = q->Q[lowest]; //swapping
        q->Q[lowest] = temp;

        *n = q->Q[q->tail]; //set pointer to tail
        q->tail += 1; //increase tail by 1
        q->current_size -= 1; //decrease current size
        return true; //return true if success
    }
    return false;
}

void pq_print(PriorityQueue *q) { //debugging print statement
    for (uint32_t i = q->tail; i < q->head; i++) {
        node_print(q->Q[i]);
    }
    printf("%d\n", pq_size(q));
}
