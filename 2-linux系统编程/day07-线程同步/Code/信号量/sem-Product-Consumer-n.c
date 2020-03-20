#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int buf[5]; //缓冲区
sem_t empty, full; //信号量
int in, out; //生产消费下标
pthread_mutex_t mutex; //线程标识符

void *consumer(void *p) { //消费者线程
    int i = 5;
    int nextc;
    while (i--) {
        sem_wait(&full); //消费者P操作
        pthread_mutex_lock(&mutex); //上锁
        nextc = buf[out]; //取出产品
        out = (out + 1) % 5; //循环取出
        pthread_mutex_unlock(&mutex); //解锁
        sem_post(&empty); //进行V操作
        printf("cosumer nextc=%d\n", nextc);
        sleep(rand() % 4); //休眠
    }
}

/*利用信号量和互斥锁实现生产者与消费者之间的同步与互斥问题，设置了缓存，共享资源有多个区间，解决了一段时间生产者产出大于消费，同时也满足了当消费者大于生产者产出的问题*/
int main() {
    int i = 5, nextp;
    pthread_t tid;
    in = 0;
    out = 0;
    srand((unsigned int)time(NULL)); //获取随机值
    sem_init(&empty, 0, 5); //初始化生产者信号量，初值为5
    sem_init(&full, 0, 0); //初始化消费者信号量，初值为0
    pthread_mutex_init(&mutex, NULL); //初始化互斥锁

    if (pthread_create(&tid, NULL, consumer, NULL) < 0) { //创建线程，线程函数为消费者函数
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    while (i--) {
        nextp = rand(); //获取随机值
        sleep(nextp % 5); //休眠
        printf("produce done nextp=%d\n", nextp);
        sem_wait(&empty); //信号量P操作
        pthread_mutex_lock(&mutex); //上锁
        buf[in] = nextp; //将产品录入缓冲区
        in = (in + 1) % 5; //实现循环存储
        pthread_mutex_unlock(&mutex); //解锁
        sem_post(&full); //信号量V操作
    }
    pthread_join(tid, NULL); //等待线程结束
    exit(0);
}