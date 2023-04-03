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
	fprintf(stderr, "  A Huffman decoder.\n");
	fprintf(stderr, "  Decompresses a file using the Huffman coding algorithm.\n\n");
	fprintf(stderr, "USAGE\n");
	fprintf(stderr, "  ./decode [-h] [-i infile] [-o outfile]\n\n");
	fprintf(stderr, "OPTIONS\n");
	fprintf(stderr, "  -h             Program usage and help.\n");
	fprintf(stderr, "  -v             Print compression statistics.\n");
	fprintf(stderr, "  -i infile      Input file to decompress.\n");
	fprintf(stderr, "  -o outfile     Output of decompressed data.\n");
}

int main(int argc, char **argv) {
	char *infile_n;
	char *outfile_n;
	int opt;
	int infile;
	int outfile;
	bool verbose = false;

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
  	if (infile_n) {
  		infile = open(infile_n, O_RDONLY);
  		if (infile < 0) {
  			return 1;
  		}
  	}
  	else {
  		infile = 0;
  	}
  	if (outfile_n) {
  		outfile = creat(outfile_n, O_WRONLY);
  	}
  	else {
  		outfile = 1;
  	}
  	if (infile >= 0) {
  		struct stat s;
		if (fstat(infile, &s) < 0) {
			return 1;
		}
  		Header *h;
  		uint8_t h_buffer[sizeof(Header)];
  		bytes_read += read_bytes(infile, h_buffer, sizeof(Header));
  		h = (Header*)&h_buffer;
  		if (h->magic != MAGIC) {
  			fprintf(stderr, "Invalid magic number.\n");
  			return 1;
  		}
  		fchmod(outfile, h->permissions);
  		uint8_t tree_bytes[h->tree_size];
  		bytes_read += read_bytes(infile, tree_bytes, h->tree_size);
  		Node *root = rebuild_tree(h->tree_size, tree_bytes);
  		if (!root) {
  			return 1;
  		}
  		uint8_t bit;
  		uint8_t buf[4096];
  		uint64_t n_bytes = 0;
  		Node *curr = root;
  		while (read_bit(infile, &bit)) {
  			if (n_bytes == 4096) {
  				write_bytes(outfile, buf, n_bytes);
  				n_bytes = 0;
  			}
  			if (!curr->right && !curr->left) {
  				buf[n_bytes] = curr->symbol;
  				bytes_written += 1;
  				n_bytes++;
  				if (h->file_size == bytes_written) {
  					break;
  				}
  				curr = root;
  			}
  			if (bit == 0) {
  				curr = curr->left;
  			}
  			else {
  				curr = curr->right;
  			}
  		}
  		write_bytes(outfile, buf, n_bytes);
  		if (verbose) {
			fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_read);
			fprintf(stderr, "Decompressed file size: %lu bytes\n", bytes_written);	
			fprintf(stderr, "Space saving: %.2f%%\n", (100 *(1 - bytes_read/(double)bytes_written)));
  		}
  		delete_tree(&root);
  		close(outfile);
  		close(infile);
  	}
  	return 0;
}
