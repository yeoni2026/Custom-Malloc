#include <stdio.h>
#include "mymalloc.h"

int main(){
    printf("1. my_free 인자에 NULL 들어갈때, 중복 free 될때 검증.\n");
    int *num = my_malloc(sizeof(int));    
    *num = 34;
    printf("%d입니다.\n", *num);
    my_free(NULL);
    print_free_list();
    my_free(num);
    print_free_list();
    my_free(num);
    print_free_list();

    //3번 검증을 위한 코드
    int *tmp = my_malloc(sizeof(int));
    int *tmp2 = my_malloc(sizeof(int));

    printf("\n2. 여러 malloc가 해제될때 인접한 해제 메모리 합병이 되는지 검증.\n");
    char *array[5];
    for (int i = 0; i < 5; ++i){
        array[i] = my_malloc(sizeof(char *));
        *array[i] = 'A' + i;
        print_free_list();
    }
    for (int i = 0; i < 5; ++i){
        printf("%c 입니다.\n", *array[i]);
        my_free(array[i]);
        print_free_list();
    }

    printf("\n3. 사이즈가 큰 malloc 할당이 들어올때 충분한 크기의 해제 메모리를 찾아가서 split 되는지 검증.\n");
    my_free(tmp);
    print_free_list();
    long *ll = my_malloc(sizeof(long) * 2);
    print_free_list();
    my_free(ll);
    print_free_list();

    return 0;
}