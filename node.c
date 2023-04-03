#include "node.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>

Node *node_create(uint8_t symbol, uint64_t frequency) {
  Node *node = (Node *)malloc(sizeof(Node)); 
  if (node) {
    node->symbol = symbol;
    node->frequency = frequency;
  }
  return node;
}

void node_delete(Node **n) {
  if (*n) {
    free(*n);
    *n = NULL;
  }
  return;
}

Node *node_join(Node *left, Node *right) {
  Node *parent = node_create('$', left->frequency + right->frequency);
  if (parent) {
    parent->left = left;
    parent->right = right;
  }
  return parent;
}

void node_print_sym(Node *n) {
  if (n) {
    if (isprint(n->symbol)) {
      printf("Symbol = %c\n", n->symbol);
    }
    if (iscntrl(n->symbol)) {
      printf("0x%02" PRIx8 "\n", n->symbol);
    }
  }
}

void node_print(Node *n) {
  if (n) {
    if (n->symbol == '$') {
      printf("Parent frequency: %lu\n", n->frequency);
      printf("L child: ");
      node_print_sym(n->left);
      printf("R child: ");
      node_print_sym(n->right);
    }
    else {
      node_print_sym(n);
    }
  }
}

bool node_cmp(Node *n, Node *m) {
  return n->frequency > m->frequency ? true : false;
}
