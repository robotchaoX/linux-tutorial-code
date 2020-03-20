#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 定义锁
pthread_mutex_t mutex; // 全局

// void err_thread(int ret, char *str) {
//     if (ret != 0) {
//         fprintf(stderr, "%s:%s\n", str, strerror(ret));
//         pthread_exit(NULL);
//     }
// }

void *tfn(void *arg) {
    srand(time(NULL));

    while (1) {
        // 加互斥锁
        pthread_mutex_lock(&mutex);
        printf("hello ");
        sleep(rand() % 3); // 模拟长时间操作共享资源，导致cpu易主，产生与时间有关的错误
        printf("world\n");
        // 解互斥锁
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 3); // 等一会 让其它线程有机会抢
    }

    return NULL;
}

// 互斥锁
int main(void) {
    srand(time(NULL)); // 随机数种子

    // 初始化互斥锁
    pthread_mutex_init(&mutex, NULL); // 初始化后mutex==1

    pthread_t tid;
    // 创建线程
    pthread_create(&tid, NULL, tfn, NULL);

    int flg = 5;
    while (flg--) { // 循环5次
        // 加互斥锁
        pthread_mutex_lock(&mutex);
        printf("HELLO ");
        sleep(rand() % 3); // 模拟长时间操作共享资源，导致cpu易主，产生与时间有关的错误
        printf("WORLD---\n");
        // 解互斥锁
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 3); // 等一会 让其它线程有机会抢
    }

    // 取消子线程
    pthread_cancel(tid); //  将子线程杀死,子线程中自带取消点
    // 回收子线程
    pthread_join(tid, NULL);

    // 销毁互斥锁
    pthread_mutex_destroy(&mutex);

    return 0; // main中的return可以将整个进程退出
}

/*线程之间共享资源stdout*/
