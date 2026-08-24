#pragma once

struct Node {
    struct Node *next;
    long block_size;
};

void *my_malloc(unsigned int size);
void my_free(void *ptr);
void print_free_list();