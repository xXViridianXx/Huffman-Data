# Project 6 - Huffman data compression

## Summary:
This program takes in a user inputed message and encodes it to aan outfile. The outfile is then taken in from the decoder and and decoded to get the original message.

## Run:
To the run the program, one can type "make all" in order to compile the whole program. You can either use the terminal or another file to encode. Pipe the file or stdin message using | ./encode (use -i if using a file). One can choose an output file using -o. Then pipe through decode and decode the message. There should be no difference between original and decoded message.

## Expected Output:
The code should output the original message typed into the file or terminal.

## Error Handling/Errors:
If the user inputs a "NULL" file to decode, then the program spits out an error. This also goes for encode. The usage function provides help when neccessary.

## Scan-build:
running "scan-build make" gives me : warning: Access to field 'left' results in a dereference of a null pointer (loaded from variable 'node'). I guess by accessing the left node, I am derefrencing the original node?

## Valgrind:
Gives no Leaks...But fails to take in very large files


