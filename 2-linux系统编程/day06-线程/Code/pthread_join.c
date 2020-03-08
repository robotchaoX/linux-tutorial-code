#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int a;
    int b;
} exit_t; // 线程间共享数据的结构体

void *tfn(void *arg) {
    exit_t *ret;

    ret = malloc(sizeof(exit_t)); // 开辟到堆上，main中可用

    ret->a = 100;
    ret->b = 300;

    // 传回指针
    pthread_exit((void *)ret); // 子线程退出时传出参数 void * 线程结束状态
    // return (void *)ret; // return 和 pthread_exit 一样，可以传回参数
}

int main(void) {
    exit_t *retval; // 接收传出参数

    pthread_t tid;
    pthread_create(&tid, NULL, tfn, NULL); // 创建线程

    // 阻塞，调用pthread_join可以获取线程的退出状态
    pthread_join(tid, (void **)&retval); // 接收 强转 void **  像进程中wait(&status);
    printf("a = %d, b = %d \n", retval->a, retval->b);

    return 0;
}
