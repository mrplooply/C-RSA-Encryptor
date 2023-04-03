#include "stack.h"
#include <stdlib.h>

 
struct Stack { 
	uint32_t top;
	uint32_t capacity;
	Node **items; 
};

Stack *stack_create(uint32_t capacity){
	Stack *s = (Stack*)malloc(sizeof(Stack));
	if (s) {
		s->top = 0;
		s->capacity = capacity;
		s->items = (Node**)malloc(capacity * sizeof(Node));
		if (!(s->items)) {
			free(s);
			s = NULL;
		}
	}
	return s;
}

void stack_delete(Stack **s) {
	if (*s) {
		free((*s)->items);
		free(*s);
		*s = NULL;
	}
}

bool stack_empty(Stack *s) {
	return s->top == 0 ? true : false;
}

bool stack_full(Stack *s) {
	return s->top == s->capacity ? true : false;
}

uint32_t stack_size(Stack *s) {
	return s->top;
}

bool stack_push(Stack *s, Node *n) {
	if (!stack_full(s)) {
		s->items[s->top] = n;
		s->top++;
		return true;
	}
	return false;
}

bool stack_pop(Stack *s, Node **n) {
	if (!stack_empty(s)) {
		s->top--;
		*n = s->items[s->top];
		return true;
	}
	return false;
}

void stack_print(Stack *s) {
	for (uint32_t i = s->top; i > 0; i--) {
		node_print(s->items[i - 1]);
	}
}
