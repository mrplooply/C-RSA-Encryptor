#include "node.h"
#include "pq.h"
#include "code.h"
#include "io.h"
#include "stack.h"
#include "huffman.h"
#include "header.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#define OPTIONS "hi:o:v"

void print_help() {
	fprintf(stderr, "SYNOPSIS\n");
	fprintf(stderr, "  A Huffman encoder.\n");
	fprintf(stderr, "  Compresses a file using the Huffman coding algorithm.\n\n");
	fprintf(stderr, "USAGE\n");
	fprintf(stderr, "  ./encode [-h] [-i infile] [-o outfile]\n\n");
	fprintf(stderr, "OPTIONS\n");
	fprintf(stderr, "  -h             Program usage and help.\n");
	fprintf(stderr, "  -v             Print compression statistics.\n");
	fprintf(stderr, "  -i infile      Input file to compress.\n");
	fprintf(stderr, "  -o outfile     Output of compressed data.\n");
}

int main(int argc, char **argv) {
	char *infile_n;
	char *outfile_n;
	int opt;
	int infile;
	int outfile;
	bool verbose = false;
	bool tmp_created = false;

  	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
  		switch (opt) {
  		case 'h':
  			print_help();
  			return 0;
  		case 'i':
  			infile_n = optarg;
  			break;
  		case 'o':
  			outfile_n = optarg;
  			break;
  		case 'v':
  			verbose = true;
  			break;
  		default:
  			print_help();
  			return 1;
  		}
  	}
  	//create temp file to store input from stdin
  	if (!infile_n) {
  		infile = creat("tmp", 33204);
  		if (infile < 0) {
  			printf("Cannot create temp file.\n");
  			return 1;
  		}
  		tmp_created = true;
		uint8_t buf[4096];
		int bytes_read;
		while ((bytes_read = read_bytes(0, buf, 4096)) > 0) {
			write_bytes(infile, buf, bytes_read);
		}
		//tried to use lseek but could not reset position of file.
		close(infile);
		infile = open("tmp", O_RDONLY);

  	}
  	//open file that is specified.
  	else {
  		infile = open(infile_n, O_RDONLY);
  		if (infile < 0) {
  			printf("Could not open the file specified: %s\n", infile_n);
  			return 1;
  		}
  	}
  	uint64_t histogram[ALPHABET] = {0};
  	uint8_t buf[4096];
  	int unique_sym = 0;
	int bytes_read;
	//make histogram
	while ((bytes_read = read_bytes(infile, buf, 4096)) != 0) {
		for (int i = 0; i < bytes_read; i++)  {
			if (histogram[buf[i]] == 0) {
				unique_sym++;
			}
			histogram[buf[i]] += 1;
		}
	}
	if (histogram[0] == 0) {
		unique_sym++;
		histogram[0] = 1;
	}
	if (histogram[1] == 0) {
		unique_sym++;
		histogram[1] = 1;
	}
	//build huffman tree
	Node *root = build_tree(histogram);
	Code table[ALPHABET];

	//need to zero out arry in order to index without seg fault.
	Code c = code_init();
	for (int i = 0; i < ALPHABET; i++) {
		table[i] = c;
	}
	build_codes(root, table);
	fchmod(infile, 33204);
	struct stat s;
	if (fstat(infile, &s) < 0) {
		return 1;
	}
	//set header
	Header h;
	h.magic = MAGIC;
	h.permissions = s.st_mode;
	h.tree_size = (unique_sym * 3) - 1;
	h.file_size = s.st_size;

	uint8_t arr[sizeof(h)];
	memcpy(arr, &h, sizeof(h));
	//if no output is selected write to stdnout
  	if (!outfile_n) {
  		outfile = 1;
  	}
  	else {
  		outfile = creat(outfile_n, 33204);
  	}
  	bytes_written += write_bytes(outfile, arr, sizeof(h));
	dump_tree(outfile, root);
	uint8_t character;
	if (!infile_n) {
		infile = open("tmp", O_RDONLY);
	}
	else {
		infile = open(infile_n, O_RDONLY);
	}
	if (infile > 1) {
		while ((bytes_read = read_bytes(infile, &character, 1)) != 0) {
			write_code(outfile, &table[character]);
		}
		flush_codes(outfile);
	}
	if (verbose) {
		fprintf(stderr, "Uncompressed file size: %lu bytes\n", s.st_size);
		fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_written);
		fprintf(stderr, "Space saving: %.2f%%\n", (100 *(1 - (double)bytes_written/s.st_size)));
	}
	close(infile);
	close(outfile);
	delete_tree(&root);
	if (tmp_created) {
		if (remove("tmp") != 0) {
			fprintf(stderr, "Could not delete temp file.\n");
		}
	}
	return 0;
}
