#include "io.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

uint64_t bytes_read;
uint64_t bytes_written;
static uint8_t write_buffer[BLOCK] = { 0 };
static int block_counter = 0;
int read_bytes(int infile, uint8_t *buf, int nbytes) {
    int bytes = 0;
    int byte_num = 0;
    while ((bytes = read(infile, buf + byte_num, nbytes - byte_num))
           > 0) { //read return 0 if at end of file or -1 for error
        byte_num += bytes;
        if (byte_num == nbytes) {
            return byte_num;
        }
    }
    return byte_num; //return total bytes read or the "final amount of bytes read"
}

int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    int bytes = 0;
    int byte_num = 0;
    while ((bytes = write(outfile, buf + byte_num, nbytes - byte_num))
           > 0) { //write return -1 if error and 0 if nothing wrong
        byte_num += bytes;
        if (byte_num == nbytes) {
            return byte_num;
        }
    }
    return byte_num; //return total bytes read or the "final amount of bytes read"
}

bool read_bit(int infile, uint8_t *bit) {
    static uint8_t buffer[BLOCK]
        = { 0 }; //static buffer set to 4096, static b/c accessable during next function call
    static int counter = 0;
    static uint32_t nbytes;
    if (counter == 0) {
        nbytes = read_bytes(infile, buffer, BLOCK);
    }
    int last = (8 * nbytes) + 1;
    if (counter == last) {
        counter = 0; //if counter reaches buffer size, reset counter
        return false;
    }
    *bit = 1 & (buffer[counter / 8] >> (counter % 8)); //set pointer *bit to the bit at the index
    counter += 1; //increment counter
    if (counter % 7 == 0) {
        bytes_read += 1;
    }
    return true;
}

void write_code(int outfile, Code *c) {
    uint8_t bit;
    uint8_t ind = 0;
    while (ind < code_size(c)) {
        bit = code_get_bit(c, ind); //get bit at the counter point
        //left shift bit add or with element in buffer
        write_buffer[block_counter / 8] |= (bit << (block_counter % 8));
        block_counter += 1; //increment buffer
        ind += 1;

        if (block_counter == BLOCK) { //if counter == top or buffer length reached, write
            flush_codes(outfile);
            block_counter = 0; //reset counter
        }
    }
}

void flush_codes(int outfile) { //only prints min bytes needed and flushes buffer
    while (block_counter % 8 != 0) {
        block_counter += 1; //increase block counter until div by 8
    }
    bytes_written += (block_counter / 8); // for stats
    write_bytes(outfile, write_buffer, block_counter / 8); //writes min amount of bytes needed
    memset(write_buffer, 0, BLOCK); //reset buffer
    block_counter = 0; //reset counter
}
