#include "huffman.h"
#include "pq.h"
#include "code.h"
#include "io.h"
#include "stack.h"
#include <stdio.h>

Node *build_tree(uint64_t hist[static ALPHABET]) {
	PriorityQueue *q =  pq_create(ALPHABET);
	for (uint32_t i = 0; i < ALPHABET; i++) {
		if (hist[i] > 0) {
			Node *n = node_create(i, hist[i]);
			enqueue(q, n);
		}
	}
	while (pq_size(q) != 1) {
		Node *n1;
		Node *n2;
		dequeue(q, &n1);
		dequeue(q, &n2);
		Node *parent = node_join(n1, n2);
		enqueue(q, parent);
	}
	Node *root;
	dequeue(q, &root);
	pq_delete(&q);
	return root;
}

static void post_order(Code c, Code table[static ALPHABET], Node *root) {
	if (!root) {
		return;
	}
	if (!root->left && !root->right) {
		table[root->symbol] = c;
		return;
	}
	uint8_t bit;
	code_push_bit(&c, 0);
	post_order(c, table, root->left);
	code_pop_bit(&c, &bit);
	code_push_bit(&c, 1);
	post_order(c, table, root->right);
	code_pop_bit(&c, &bit);
}

void build_codes(Node *root, Code table[static ALPHABET]) {
	Code c = code_init();
	post_order(c, table, root);
}

void dump_tree(int outfile, Node *root) {
	if (!root) {
		return;
	}
	dump_tree(outfile, root->left);
	dump_tree(outfile, root->right);
	if (!root->left && !root->right) {
		uint8_t bytes[2] = {'L', root->symbol};
		bytes_written += write_bytes(outfile, bytes, 2);
	}
	else {
		uint8_t bytes[1] = {'I'};
		bytes_written += write_bytes(outfile, bytes, 1);
	}


}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
	Stack *s = stack_create(nbytes);
	for (uint64_t i = 0; i < nbytes; i++) {
		if (tree[i] == 'L') {
			Node *n = node_create(tree[i + 1], 1);
			stack_push(s, n);
			i++;
			continue;
		}
		if (tree[i] == 'I') {
			Node *right;
			Node *left;
			stack_pop(s, &right);
			stack_pop(s, &left);
			Node *parent = node_join(left, right);
			stack_push(s, parent);
		}
	}
	Node *root;
	stack_pop(s, &root);
	stack_delete(&s);
	return root;
}

void delete_tree(Node **root) {
	if (!(*root)) {
		return;
	}
	delete_tree(&(*root)->left);
	delete_tree(&(*root)->right);
	if (!((*root)->left) && !((*root)->right)) {
		node_delete(&(*root));
	}
	*root = NULL;
}
