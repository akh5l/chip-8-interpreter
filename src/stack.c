#include "../include/stack.h"

#include <stdio.h>

void stack_initialize(Stack* s) {
  s->top = -1;
}

void stack_push(Stack* s, const void* data) {

  if (s->top >= MAX_STACK_SIZE - 1) {
    printf("Stack overflow at %p!\n", s);
    return;
  }

  s->arr[++s->top] = (void*)data;
}

void* stack_pop(Stack* s) {
  if (s->top == -1) {
    printf("Stack at %p is empty but pop was called!\n", s);
    return NULL;
  }

  return s->arr[s->top--];

}

void* stack_peek(Stack* s) {
  if (s->top == -1) {
    printf("Stack at %p is empty but peek was called!\n", s);
    return NULL;
  }

  return s->arr[s->top];
}