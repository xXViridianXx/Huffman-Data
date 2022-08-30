#include "code.h"
#include <stdio.h>

Code code_init(void) {
    Code code = { .top = 0, .bits = { 0 } }; //init struct
    return code;
}

uint32_t code_size(Code *c) {
    return c->top; //since adding 1 after pushing, code_size == top
}

bool code_empty(Code *c) { //code empty if top == 0
    return c->top == 0;
}

bool code_full(Code *c) { //code full if all 256 bits used
    return code_size(c) == 256;
}

bool code_set_bit(Code *c, uint32_t i) {
    if (i > 255) { // if index > 255 can't set bit; codes = array 256/8
        return false; // because 256 is extended ascii table
    }
    /*uses division to get index of bits
     * shifts the 1 using mod 8
     * using |= because want to only set that bit
     * and nothing else
     * did right shift before but changed to left
     * after Audrey said it was easier*/
    c->bits[i / 8] |= (1 << (i % 8));
    return true;
}

bool code_clr_bit(Code *c, uint32_t i) {
    if (i > 255) {
        return false;
    }
    /*Same premise but doing &= with complement
     * because only want to clear the bit and
     * not touch anything else*/
    c->bits[i / 8] &= ~(1 << i % 8);
    return true;
}

bool code_get_bit(Code *c, uint32_t i) {
    if (i > 255) {
        return false;
    }
    /*Since don't want to change code, return right shit
     * & with 1
     * If 1 at that spot return 1 else 0*/
    return 1 & (c->bits[i / 8] >> (i % 8));
}

bool code_push_bit(Code *c, uint8_t bit) { //push to least significant?
    if (code_full(c)) {
        return false;
    }
    if (bit == 1) {
        code_set_bit(c, c->top); //sets bit to 1
    } //if bit is 0, just move top by 1 since everything already 0
    if (bit == 0) {
        code_clr_bit(c, c->top); //not needed but eh
    }
    c->top += 1; //increase top by 1 since pushing; like stack
    return true;
}

bool code_pop_bit(Code *c, uint8_t *bit) { //pops like stack
    if (code_empty(c)) {
        return false;
    }
    c->top -= 1; //subtracts 1 from top
    *bit = code_get_bit(c, c->top); //sets bit to what top was at
    return true;
}

void code_print(Code *c) { //code_print for debugging, along with main
    for (uint32_t i = 0; i < code_size(c); i++) {
        printf("%d, ", c->bits[i]);
    }
    printf("\n");

    /*For debugging*/

    /*uint32_t test = 17;
	uint32_t index = test / 8;
	printf("%d\n", c->top);
	printf("%d\n", c->bits[index]);
	code_set_bit(c, test);
	printf("setting bit %d at index %d\n", c->bits[index], index);
	code_get_bit(c, test);
	printf("getting bit %d at index %d\n", c->bits[index], index);
	code_clr_bit(c, test);
	printf("clearing bit %d at index %d\n", c->bits[index], index);
	printf("\n");
	printf("pushing bit..1\n");
	code_push_bit(c, 1);
	index = (c->top -1) /8;
	printf("bit at index %d = %d\n", c->top-1, c->bits[index]);
	printf("pushing bit..0\n");
	code_push_bit(c, 0);
	printf("bit at index %d = %d\n", c->top-1, c->bits[index]);
	printf("pushing bit...1\n");
	code_push_bit(c, 1);
	printf("bit at index %d = %d\n", c->top-1, c->bits[index]);
	printf("pushing bit...0\n");
	code_push_bit(c, 0);
	printf("bit at index %d = %d\n", c->top-1, c->bits[index]);
	printf("\n");
	uint8_t popped;
	code_pop_bit(c, &popped);
	printf("popping bit...1\n");
	printf("popped bit = %d\n", popped);
	code_pop_bit(c, &popped);
	printf("popping bit...0\n");
	printf("popped bit = %d\n", popped);
	code_pop_bit(c, &popped);
	printf("popping bit...1\n");
	printf("popped bit = %d\n", popped);
	code_pop_bit(c, &popped);
	printf("popping bit...0\n");
	printf("top = %d\n", c->top);
	printf("top = %d\n", c->top);
	printf("popped bit = %d\n", popped);*/
}

/*int main(void) {

	Code code = code_init();
	code_print(&code);
	//printf("%d\n", code.top);
	//printf("%d\n", code.bits[0]);
	//code_set_bit(&code, 5);
	//printf("setting bit %d\n", code.bits[0]);
	//code_get_bit(&code, 5);
	//printf("getting bit %d\n", code.bits[0]);
	//code_clr_bit(&code, 5);
	//printf("clearing bit %d\n", code.bits[0]);
	return 0;
}*/
