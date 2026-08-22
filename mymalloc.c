#include "mymalloc.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

static struct Node *head = NULL;

void *my_malloc(unsigned int size){
    if (!size){
        printf("Size cannot be zero.\n");
        exit(1);
    }
    if (head == NULL){
        head = sbrk(sizeof(struct Node));
        head->next = NULL;
    }
    
    long block_size = ((size - 1) / 8 + 1) * 2;
    long *ptr;
    struct Node *curr = head;
    int flag = 0;

    while (curr->next != NULL){
        if (curr->next->block_size >= block_size){
            ptr = (long *)curr->next;
            if (curr->next->block_size > block_size){
                struct Node *tmp_node = (struct Node *)((char*)curr->next + block_size * 8); //포인터 연산은 포인터 타입에 따라 이동하는 바이트 수가 바뀌기에 char*로 캐스팅한 이후에 연산하기
                tmp_node->next = curr->next->next;
                tmp_node->block_size = curr->next->block_size - block_size;
                curr->next = tmp_node;
            }
            else {
                curr->next = curr->next->next;
            }
            flag = 1;
            break;
        }
        curr = curr->next;
    }
    if (!flag){
        ptr = sbrk(block_size * 8);
    }
    *ptr = block_size;
    ptr = (char *)ptr + 8;
    return (void *)ptr;
}

void my_free(void *ptr){
    if (head == NULL){
        head = sbrk(sizeof(struct Node));
        head->next = NULL;
    }
    struct Node *curr = head;
    while(curr->next != NULL){
        curr = curr->next;
    }

    //해제한 주소를 연결리스트로 연결
    ptr -= 8;
    long *block_size = (long *)ptr;
    curr->next = ptr;
    curr = curr->next;
    curr->next = NULL;
    curr->block_size = *block_size;

    return;
}