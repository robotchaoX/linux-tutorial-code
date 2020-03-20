#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM 5 // 存放产品容器的大小

// 共享缓冲区
int queue[NUM]; //全局数组实现环形队列

// 信号量
sem_t blank_number; // 空格子信号量
sem_t product_number; // 产品数信号量

void *producer(void *arg) {
    int i = 0;
    while (1) {
        // 空格子数--
        sem_wait(&blank_number); // >0生产者将空格子数--,为0则阻塞等待,消费了有空格再生产
        queue[i] = rand() % 1000 + 1; //生产一个产品，入队
        printf("++++Produce--i=%d---%d\n", i, queue[i]);
        // 产品数++
        sem_post(&product_number); //将产品数++

        // 环形队列,先入先出
        i = (i + 1) % NUM; //借助下标实现环形
        sleep(rand() % 2); // 生产消费速度不一样
    }
}

void *consumer(void *arg) {
    int i = 0;
    while (1) {
        // 产品数--
        sem_wait(&product_number); // >0消费者将产品数--,为0则阻塞等待,生产者生产了产品再消费
        printf("----Consume--i=%d---%d\n", i, queue[i]);
        queue[i] = 0; //消费一个产品，出队
        // 空格子数++
        sem_post(&blank_number); //消费掉以后,将空格子数++

        // 环形队列,先入先出
        i = (i + 1) % NUM; //借助下标实现环形
        sleep(rand() % 3);
    }
}

//信号量实现 生产者 消费者问题,一个生产者,一个消费者
int main(int argc, char *argv[]) {
    pthread_t pid, cid;

    // 初始化信号量 , 信号量始终满足: blank_number + product_number = NUM
    sem_init(&blank_number, 0, NUM); // 空格子信号量为 5
    sem_init(&product_number, 0, 0); // 产品数信号量为 0

    // 创建子线程
    pthread_create(&pid, NULL, producer, NULL);
    pthread_create(&cid, NULL, consumer, NULL);

    // 回收线程
    pthread_join(pid, NULL);
    pthread_join(cid, NULL);

    // 销毁信号量
    sem_destroy(&blank_number);
    sem_destroy(&product_number);

    return 0;
}
