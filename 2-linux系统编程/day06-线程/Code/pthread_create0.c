#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *tfn(void *arg) {
    printf("I'm thread, Thread_ID = %lu,  my pid = %d\n", pthread_self(), getpid()); // 获取当前线程ID
    return NULL;
}

// 创建一个新线程
int main(void) {
    pthread_t tid;

    printf("I am main1, Thread_ID = %lu,  my pid = %d\n", pthread_self(), getpid());
    // 创建线程
    pthread_create(&tid, NULL, tfn, NULL);
    sleep(1);
    printf("I am main2, Thread_ID = %lu,  my pid = %d\n", pthread_self(), getpid());

    return 0;
}
