#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *thrd_func(void *arg) {
    pthread_exit((void *)77);
}

int main(void) {
    pthread_t tid;
    int ret;
    pthread_attr_t attr;

    // 初始化线程属性
    ret = pthread_attr_init(&attr); // 成功0,失败错误号
    if (ret != 0) {
        fprintf(stderr, "pthread_init error:%s\n", strerror(ret));
        exit(1);
    }
    // 设置分离状态
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    // 创建线程
    ret = pthread_create(&tid, &attr, thrd_func, NULL); // 创建的线程就为分离态
    if (ret != 0) {
        fprintf(stderr, "pthread_create error:%s\n", strerror(ret));
        exit(1);
    }
    printf("-----------------detach后无法join---------------\n");
    // detach 无法join
    ret = pthread_join(tid, NULL);
    if (ret != 0) {
        fprintf(stderr, "pthread_join error:%s\n", strerror(ret));
        // exit(1);
    }
    printf("---------------------join ret = %d\n", ret);

    // 销毁线程属性锁占用的资源
    pthread_attr_destroy(&attr); // 成功0,失败错误号

    pthread_exit((void *)1); // 结束主控线程

    // printf("--------------永不执行------------------\n");
    // return 0;
}
