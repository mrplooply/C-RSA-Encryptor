#include "io.h"
#include <unistd.h>

uint64_t bytes_read = 0;
uint64_t bytes_written = 0;

static uint64_t bit_num = 0;
static uint64_t my_bytes_read = 0;

static uint64_t my_bytes_written = 0;

static uint8_t c_buffer[BLOCK] = {0};
static uint64_t c_bit_num = 0;

int read_bytes(int infile, uint8_t *buf, int nbytes) {
	int b_read = 0;
	int local_bytes = 0;
	while (nbytes - b_read > 0) {
		local_bytes = read(infile, buf + b_read, nbytes - b_read);
		if (local_bytes <= 0) {
			return b_read;
		}
		b_read += local_bytes;
	}
	return b_read;
}

int write_bytes(int outfile, uint8_t *buf, int nbytes) {
	int w_bytes = 0;
	int bytes = 0;
	while (true) {
		w_bytes = write(outfile, buf + my_bytes_written, nbytes);
		bytes += w_bytes;
		if (w_bytes == nbytes) {
			my_bytes_written = 0;
			return bytes;
		}
		my_bytes_written += bytes;
		if (w_bytes <= 0) {
			my_bytes_written = 0;
			return bytes;
		}
	}
	return bytes; 
}

bool read_bit(int infile, uint8_t *bit) {
	static uint8_t r_buffer[BLOCK];
	if (bit_num == 0 || bit_num  == (my_bytes_read * 8)) {
		my_bytes_read = read_bytes(infile, r_buffer, BLOCK);
		bytes_read += my_bytes_read;
		if (my_bytes_read == 0) {
			return false;
		}
		bit_num = 0;
	}
	// read each byte from least sig bit to most
	if ((r_buffer[bit_num / 8] & (uint8_t)1 << (bit_num % 8)) == 0) {
		*bit = 0;
	}
	else {
		*bit = 1;
	}
	bit_num++;
	return true;
}

void write_code(int outfile, Code *c) {
	uint8_t i = 0;
	uint8_t top = code_size(c);
	while (true) {
		//if the buffer is full write to file
		if (c_bit_num == BLOCK * 8) {
			bytes_written += write_bytes(outfile, c_buffer, c_bit_num / 8);
			c_bit_num = 0;
		}
		//if the bit is 1, set the bit, else clear the bit
		if (code_get_bit(c, i)) {
			c_buffer[c_bit_num / 8] |= (uint8_t)1 << (c_bit_num % 8);
		}
		else {
			c_buffer[c_bit_num / 8] &= ~((uint8_t)1 << (c_bit_num % 8));
		}
		c_bit_num++;
		if (top == i) {
			break;
		}
		i++;
	}
}

void flush_codes(int outfile) {
	while ((c_bit_num % 8) != 0) {
		c_buffer[c_bit_num / 8] &= ~((uint64_t)1 << (c_bit_num % 8));
		c_bit_num++;
	}
	bytes_written += write_bytes(outfile, c_buffer, c_bit_num / 8);
}
