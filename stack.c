#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

struct Stack { //struct for stack
    int top;
    int capacity;
    Node **items;
};

Stack *stack_create(uint32_t capacity) {

    Stack *stack = malloc(sizeof(Stack)); //allocate memory size of stack
    if (stack) {
        stack->top = 0;
        stack->capacity = capacity;
        stack->items = (Node **) calloc(stack->capacity, sizeof(Node)); //allocate array of nodes
        if (stack->items) {
            return stack;
        }
        free(stack);
        stack = NULL; //if allocation failed, stop
    }
    return (Stack *) 0;
}

void stack_delete(Stack **s) { //frees and sets stack to NULL
    free((*s)->items);
    (*s)->items = NULL;

    free(*s);
    *s = NULL;
}

bool stack_empty(Stack *s) {
    return s->top == 0; // once top == 0, stack empty
}

bool stack_full(Stack *s) {
    return s->top == s->capacity; //full stack == capacity
}

uint32_t stack_size(Stack *s) { //stack size is top
    return s->top;
}

bool stack_push(Stack *s, Node *n) { //pushes item to stack
    if (s) {
        if (stack_full(s)) {
            return false;
        }
        s->items[s->top] = n; //set item == to top
        s->top += 1; //add 1 to top
        return true;
    }
    return false;
}

bool stack_pop(Stack *s, Node **n) {
    if (s) {
        if (stack_empty(s)) {
            return false;
        }
        s->top -= 1;
        *n = s->items[s->top]; //set *n to top
        return true;
    }
    return false;
}

void stack_print(Stack *s) { //debugging function for stack
    for (int i = 0; i < s->top; i++) {
        node_print(s->items[i]);
    }
    printf("%d\n", stack_size(s));
}
