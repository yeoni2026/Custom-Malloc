#include "mymalloc.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


static struct Node *head = NULL;

void *my_malloc(unsigned int size){
    //size 검증
    if (!size){
        printf("Size cannot be zero.\n");
        exit(1);
    }

    //head 포인터 할당. (초기화)
    if (head == NULL){
        head = sbrk(sizeof(struct Node));
        head->next = NULL;
    }
    
    //block_size 계산 (메타데이터 8바이트 + 실제데이터)
    long block_size = (size - 1) / 8 + 2;
    if (block_size % 2 == 1) block_size++; //block_size가 홀수일시, free이후 밑의 while문에서 재할당될시 1개블럭이 미아상태로 낭비될 수 있으므로 짝수로 강제

    long *ptr;
    struct Node *curr = head;
    int flag = 0;

    //해제된 힙 메모리 재사용
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

    //기존 메모리 공간에 공간이 없을시 새로 할당.
    if (!flag){
        ptr = sbrk(block_size * 8);
        if (ptr == (void *)-1){
            printf("힙 메모리 할당 실패\n");
            exit(1);
        }
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
    ptr = (char *)ptr - 8;
    long block_size = *(long *)ptr;

    //curr을 연결리스트 내에서 주소상 사이 위치에. & 동시에 리스트 합병.
    struct Node *curr = head;
    while (curr->next != NULL){
        if ((uintptr_t)ptr < (uintptr_t)curr->next){
            if ((uintptr_t)curr + curr->block_size * 8 == (uintptr_t)ptr){
                curr->block_size += block_size;
                if ((uintptr_t)ptr + block_size * 8 == (uintptr_t)curr->next){
                    curr->block_size += curr->next->block_size;
                    curr->next = curr->next->next;
                }
                break;
            }
            if ((uintptr_t)ptr + block_size * 8 == (uintptr_t)curr->next){
                long tmp_block_size = curr->next->block_size + block_size;
                struct Node *tmp_next = curr->next->next;
                curr->next = (struct Node *)((char*)curr->next - block_size * 8);
                curr->next->block_size = tmp_block_size;
                curr->next->next = tmp_next;
                break;
            }
            struct Node *tmp = (struct Node *)ptr;
            tmp->next = curr->next;
            tmp->block_size = block_size;
            curr->next = tmp;
            break;
        }
        curr = curr->next;
    }
    if (curr->next == NULL){
        curr->next = (struct Node *)ptr;
        curr->next->next = NULL;
        curr->next->block_size = block_size;
    }
    return;
}