# Assignment 7 - A (Huffman Coding) Tree Grows in Santa Cruz

## Introduction

This program has two main files, encode and decode. When used in tandom they can encode and decode files based on the huffman coding algorithm. It can be used with the standard input/output or files specified by the user.

## Building
To build the executable file use the commands..

> make

or

> make all

To compile both encode and decode. Or you can individually compile them with...

> make encode

> make decode

## Running

In order the run encoder you will need a file that you wish to encode, or you can encode text inputted into standard input.

### encode

To run encode you may or may not provide an argument.

Prints out the program usage.

> ./encode -h

Specifies the file that you wish to encode.

> ./encode -i [file name]

Specifies the output for the encodded file.

> ./encode -o [file name]

Enable verbose, in order to print statistics including the orginal file size and the encodded file size.

> ./encode -v

### decode

To run encode you may or may not provide an argument.

Prints out the program usage.

> ./decode -h

Specifies the file that you wish to encode.

> ./decode -i [file name]

Specifies the output for the encodded file.

> ./decode -o [file name]

Enable verbose, in order to print statistics including the orginal file size and the encodded file size.

> ./decode -v


## Contributing

code.c decode.c encode.c huffman.c io.c MakeFile node.c pq.c stack.c  - Nicholas Barsi-Rhyne 11/06/2022

code.h defines.h header.h huffman.h io.h node.h pq.h stack.h - Prof. Ethan L. Miller, CSE 13S – Fall 2022
