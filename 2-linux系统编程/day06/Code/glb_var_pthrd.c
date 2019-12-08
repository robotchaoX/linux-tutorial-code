#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 线程间共享全局变量
int var = 100;

void *tfn(void *arg) {
    var = 200;
    printf("I'm thread\n");

    return NULL;
}

int main(void) {
    printf("At first var = %d\n", var);

    pthread_t tid;
    pthread_create(&tid, NULL, tfn, NULL); // 创建线程修改全局变量
    sleep(1);

    printf("after pthread_create, var = %d\n", var);

    return 0;
}
