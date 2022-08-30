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

static uint8_t buffer[BLOCK];

void usage(char *exec) { /*usage error function which I reused from last assignment*/
    fprintf(stderr,
        "\n"
        "Synopsis:\n"
        "----takes in input from user and decompresses it using Huffman tree"
        "\n"
        "USAGE\n %s./decode [-hv] -i infile -o output\n"
        "\n" /*all command line options listed*/
        "OPTIONS\n"
        "    -h        help and usage\n"
        "    -v        prints compression stats mode\n"
        "    -i        Specifies input file to decode(default stdin)\n"
        "    -o        specifies output file to decode to (default stdout)\n"
        "\n",
        exec);
    return;
}

bool is_seekable(int infile) {
    //explained in encode.c
    return lseek(infile, 0, SEEK_SET) != -1;
}

int hacky_mkstemp(int infile) {
    /*already explained in encode.c
     * credit goes to Eugene*/
    int bytes = 0;
    if (!is_seekable(infile)) {
        int outfile = open("tmp.dat", O_CREAT | O_TRUNC | O_RDWR, 0600);
        unlink("tmp.dat");
        bytes = read_bytes(infile, buffer, BLOCK);
        write_bytes(outfile, buffer, bytes);
        lseek(outfile, 0, SEEK_SET);
        close(infile);
        infile = outfile;
    }

    return infile;
}

int main(int argc, char **argv) {

    int opt;
    bool verbose, in, out;
    verbose = in = out = false;
    int encoded_message; //init vars
    int decoded_message;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) { //get opt
        switch (opt) {
        case 'v': verbose = true; break;
        case 'i':
            in = true;
            encoded_message = open(optarg, O_RDONLY, 0600);
            if (encoded_message == -1) {
                fprintf(stderr, "ERROR: Nothing in File:\n"); //checking if file empty
                fprintf(stderr, "Try a Different File\n");
                return EXIT_FAILURE;
            }
            break;
        case 'o':
            out = true;
            decoded_message = open(optarg, O_CREAT | O_TRUNC | O_WRONLY, 0600);
            break;
        default:
            usage(argv[0]);
            return EXIT_FAILURE;
            break;
        }
    }
    if (!in) {
        encoded_message = hacky_mkstemp(STDIN_FILENO); //setting defaults
    }
    if (!out) {
        decoded_message = STDOUT_FILENO;
    }
    Header header;
    //reading header
    bytes_read += read_bytes(encoded_message, (uint8_t *) &header, sizeof(Header));
    if (header.magic != MAGIC) {
        //checks if file decodable
        fprintf(stderr, "This file can't be decoded\n");
        return EXIT_FAILURE;
    }
    if (out) {
        //sets permission of outfile if user chose file
        fchmod(decoded_message, header.permissions);
    }

    uint8_t tree_dump[header.tree_size]; //buffer of tree dump
    //reading tree dump
    bytes_read += read_bytes(encoded_message, tree_dump, header.tree_size);
    Node *root, *node;
    root = rebuild_tree(header.tree_size, tree_dump);
    uint8_t bit = 0;
    uint64_t leaves = 0;
    node = root;
    uint8_t message[header.file_size]; //buffer of tree dump
    while (leaves < header.file_size) {
        //if bit 0, go left else right
        read_bit(encoded_message, &bit);
        if (bit == 0) {
            node = node->left;
        } else {
            node = node->right;
        }

        if (node->left == NULL && node->right == NULL) {
            //write the node symbol to outfile
            message[leaves] = node->symbol;
            leaves += 1;
            node = root;
        }
    }
    write_bytes(decoded_message, message, header.file_size);
    if (verbose) {
        //for verbose stats
        bytes_written = header.file_size;
        fprintf(stderr, "Compressed file: %ld bytes\n", bytes_read);
        fprintf(stderr, "Decompressed file: %ld bytes\n", bytes_written);
        float stats = 100 * (1 - (bytes_read / (float) bytes_written));
        fprintf(stderr, "Space saving: %.2f%%\n", stats);
    }

    //delete allocated stuff
    delete_tree(&root);
    close(encoded_message);
    close(decoded_message);
    return 0;
}
