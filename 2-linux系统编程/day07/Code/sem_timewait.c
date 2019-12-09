#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define N 1024

// 信号量  // 升级版的锁
sem_t s;

void *tfn(void *arg) {
    char buf[N];

    while (1) {
        // 当键盘读到数据时，
        read(STDIN_FILENO, buf, N); // 阻塞读键盘输入
        printf("------get input-------\n");
        sem_post(&s); // 唤醒主线程
    }

    return NULL;
}

int main(void) {
    pthread_t tid;

    struct timespec t = {0, 0};

    // 信号量初始化
    sem_init(&s, 0, 0);

    // 创建子线程
    pthread_create(&tid, NULL, tfn, NULL);

    t.tv_sec = time(NULL) + 1; // 当前绝对时间+1秒
    t.tv_nsec = 0;

    while (1) {

        // 定时阻塞
        sem_timedwait(&s, &t); // 当子线程有输入时，会被立刻唤醒
        printf(" hello world\n"); // 每隔5s打印一次

        t.tv_sec = time(NULL) + 5; // 当前绝对时间+5秒
        t.tv_nsec = 0;
    }

    pthread_join(tid, NULL);
    // 销毁信号量
    sem_destroy(&s);

    return 0;
}
