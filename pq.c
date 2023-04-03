#include "pq.h"
#include "node.h"
#include <stdlib.h>

struct PriorityQueue {
	uint32_t capacity;
	uint32_t size;
	Node **array;
};

static uint32_t l_child(uint32_t n) {
	return (2 * n + 1);
}

static uint32_t r_child(uint32_t n) {
	return (2 * n + 2);
}

static uint32_t parent(uint32_t n) {
	return (n - 1) / 2;
}

static void swap(PriorityQueue *q, uint32_t index1, uint32_t index2) {
	Node *temp = q->array[index1];
	q->array[index1] = q->array[index2];
	q->array[index2] = temp;
}

static void up_heap(PriorityQueue *q) {
	if (q->size == 0) {
		return;
	}
	uint32_t n_parent = parent(q->size);
	uint32_t n = q->size;
	while (node_cmp(q->array[n_parent], q->array[n])) {
		swap(q, n, n_parent);
		if (n_parent == 0) {
			break;
		}
		n = n_parent;
		n_parent = parent(n_parent);
	}
}

static void down_heap(PriorityQueue *q) {
	uint32_t n = 0;
	uint32_t smaller;
	while (l_child(n) < q->size) {
		if (r_child(n) == q->size) {
			smaller = l_child(n);
		}
		else {
			if (!node_cmp(q->array[l_child(n)], q->array[r_child(n)])) {
				smaller = l_child(n);
			}
			else{
				smaller = r_child(n);
			}
		}
		if (!node_cmp(q->array[n], q->array[smaller])) {
			break;
		}
		swap(q, n, smaller);
		n = smaller;
	}
}

PriorityQueue *pq_create(uint32_t capacity) {
	PriorityQueue *pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
	if (pq) {
		pq->capacity = capacity;
		pq->array = (Node**)malloc(capacity * sizeof(Node));
		if (!(pq->array)) {
			free(pq);
			pq = NULL;
		}
		pq->size = 0;
	}
	return pq;
}

void pq_delete(PriorityQueue **q) {
	if (*q) {
		free((*q)->array);
		free(*q);
	}
}

bool pq_empty(PriorityQueue *q) {
	return q->size == 0 ? true : false;
}

bool pq_full(PriorityQueue *q) {
	return q->size == q->capacity ? true : false;
}

uint32_t pq_size(PriorityQueue *q) {
	return q->size;
}

bool enqueue(PriorityQueue *q, Node *n) {
	if (!pq_full(q)) {
		q->array[q->size] = n;
		up_heap(q);
		q->size++;
		return true;
	}
	return false;
}

bool dequeue(PriorityQueue *q, Node **n) {
	if (!pq_empty(q)) {
		*n = q->array[0];
		swap(q, 0, q->size - 1);
		q->size--;
		down_heap(q);
		return true;
	}
	return false;
}

void pq_print(PriorityQueue *q) {
	for (uint32_t i = 0; i < q->size; i++) {
		node_print_sym(q->array[i]);
	}
}
