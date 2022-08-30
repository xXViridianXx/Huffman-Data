#include "header.h"
#include "io.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "defines.h"
#include "huffman.h"
#define OPTIONS "vhi:o:"

static uint8_t buffer[BLOCK]; //buffer to store message

void usage(char *exec) { /*usage error function which I reused from last assignment*/
    fprintf(stderr,
        "\n"
        "Synopsis:\n"
        "----takes in input from user and compresses it using Huffman compression"
        "\n"
        "USAGE\n %s./encode [-hv] -i infile -o output\n"
        "\n" /*all command line options listed*/
        "OPTIONS\n"
        "    -h        help and usage\n"
        "    -v        prints compression stats mode\n"
        "    -i        Specifies input file to encode(default stdin)\n"
        "    -o        specifies output file to encode to (default stdout)\n"
        "\n",
        exec);
    return;
}

bool is_seekable(int infile) {
    return lseek(infile, 0, SEEK_SET) != -1; //checks if user used stdin
}

int hacky_mkstemp(int infile) {
    /*credit for this function and is_seekable 
     * goes to Eugene. Attended Eugene's section
     * and did similar funstions*/

    //if user did stdin, create temp file
    int bytes = 0;
    if (!is_seekable(infile)) {
        int outfile = open("tmp.dat", O_CREAT | O_TRUNC | O_RDWR, 0600); //create temp file
        unlink("tmp.dat"); //remove tmp when closed
        bytes = read_bytes(infile, buffer, BLOCK); //read into buffer
        write_bytes(outfile, buffer, bytes); //write to an outfile
        lseek(outfile, 0, SEEK_SET); //reset
        close(infile);
        infile = outfile; //setting infile = outfile
    }

    return infile;
}

int main(int argc, char **argv) {

    int opt;
    bool verbose, in, out;
    verbose = in = out = false; //init variables
    int orig_message;
    int encoded_message;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) { //simple get opt
        switch (opt) {
        case 'v': verbose = true; break;
        case 'i':
            in = true;
            orig_message = open(optarg, O_RDONLY, 0600);
            if (orig_message == -1) {
                fprintf(stderr, "ERROR: Nothing in File:\n"); //checking if file empty
                fprintf(stderr, "Try a Different File\n");
                return EXIT_FAILURE;
            }
            break;
        case 'o':
            out = true;
            encoded_message = open(optarg, O_CREAT | O_TRUNC | O_WRONLY, 0600);
            break;
        default:
            usage(argv[0]);
            return EXIT_FAILURE;
            break;
        }
    }
    if (!in) {
        //if user used stdin, create temp file
        orig_message = hacky_mkstemp(STDIN_FILENO);
    }
    if (!out) {
        //setting default outfile
        encoded_message = STDOUT_FILENO;
    }
    struct stat file;
    fstat(orig_message, &file); //getting file stats

    uint8_t message[file.st_size]; //read into here
    uint64_t histogram[ALPHABET] = { 0 }; //init hist
    histogram[0] = 1;
    histogram[255] = 1; //setting for error purposes
    read_bytes(orig_message, message, file.st_size); //read and store
    for (int i = 0; i < file.st_size; i++) {
        histogram[message[i]] += 1; //creating hist
    }

    Node *root;
    root = build_tree(histogram); //building tree
    Code table[ALPHABET] = { 0 };
    build_codes(root, table); //storing codes in table

    uint32_t counter = 0;
    //counts number of unique symbols
    for (uint32_t i = 0; i < 256; i++) {
        if (histogram[i] > 0) {
            counter += 1;
        }
    }
    Header header = { //creating header with stats
        .magic = MAGIC,
        .permissions = 0600,
        .tree_size = 3 * counter - 1,
        .file_size = file.st_size
    };
    //writing header to encoded message
    bytes_written += write_bytes(encoded_message, (uint8_t *) &header, sizeof(Header));
    dump_tree(encoded_message, root); //dumping tree to encoded message
    lseek(orig_message, 0, SEEK_SET); //reseting file
    //writing codes to encoded_message file
    for (uint32_t i = 0; i < sizeof(message) / sizeof(message[0]); i++) {
        write_code(encoded_message, &(table[message[i]]));
    }

    flush_codes(encoded_message); //flushing
    if (verbose) { //stats fro verbose mode
        bytes_read = file.st_size;
        fprintf(stderr, "Uncompressed file: %ld bytes\n", bytes_read);
        fprintf(stderr, "Compressed file: %ld bytes\n", bytes_written);
        float stats = 100 * (1 - (bytes_written / (float) bytes_read));
        fprintf(stderr, "Space saving: %.2f\n", stats);
    }
    delete_tree(&root);
    close(orig_message); //closing stuff
    close(encoded_message);
    return 0;
}
