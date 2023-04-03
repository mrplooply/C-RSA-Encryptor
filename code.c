#include "code.h"
#include <stdio.h>

Code code_init(void) {
	Code c;
	c.top = 0;
	for (uint8_t i = 0; i < MAX_CODE_SIZE; i++) {
		c.bits[i] = 0;
	}
	return c;
} 

uint32_t code_size(Code *c) {
	return c->top == 0 ? 0 : c->top - 1;
}

bool code_empty(Code *c) {
	return c->top == 0 ? true : false;
}

bool code_full(Code *c) {
	return c->top == MAX_CODE_SIZE ? true : false;
}

bool code_set_bit(Code *c, uint32_t i) {
	if (i <= c->top) {
		c->bits[i / 8] |= (uint64_t)1 << (i % 8);
		return true;
	}
	return false;
}

bool code_clr_bit(Code *c, uint32_t i) {
	if (i <= c->top) {
		c->bits[i / 8] &= ~((uint64_t)1 << (i % 8));
		return true;
	}
	return false;
}

bool code_get_bit(Code *c, uint32_t i) {
	if (i <= c->top) {
		if ((c->bits[i / 8] & (uint64_t)1 << (i % 8)) == 0) {
    		return false;
    	}
  		return true;
	}
	return false;
}

bool code_push_bit(Code *c, uint8_t bit) {
	if (!code_full(c)) {
		if (bit == 1) {
			code_set_bit(c, c->top);
		}
		else {
			code_clr_bit(c, c->top);

		}
		c->top++;
		return true;
	}
	return false;
}

bool code_pop_bit(Code *c, uint8_t *bit) {
	if (!code_empty(c)) {
		if (code_get_bit(c, c->top - 1)) {
			*bit = 1;
		}
		else {
			*bit = 0;
		}
		code_clr_bit(c, c->top - 1);
		c->top--;
		return true;
	}
	return false;
}

void code_print(Code *c) {
	uint32_t start = code_size(c) / 8;
	if (c->top == 0) {
		printf("0\n");
		return;
	}
	while (true) {
    	// start from most significant bit to make printing easier
    	uint8_t mask = 1 << start;
  		if (c->bits[start / 8] & mask) {
    		printf("1");
  		} else {
    		printf("0");
  		}
    	if (start == code_size(c)) {
    		printf("\n");
    		break;
    	}
    	start++;
  	}
}
