/*借助条件变量模拟 生产者-消费者 问题*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*链表作为公享数据,需被互斥量保护*/
struct msg { // 节点
    struct msg *next;
    int num;
};

struct msg *head; // 头指针
struct msg *mp; // 节点指针

// 静态初始化 一个条件变量
pthread_cond_t has_product = PTHREAD_COND_INITIALIZER; // 定义时直接初始化,省init
// 静态初始化 一个互斥锁
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // 定义时直接初始化,省init

void *consumer(void *p) {
    for (;;) { // 死循环，一直消费
        // 读写共享数据（链表）
        // 加锁
        pthread_mutex_lock(&lock);
        while (head == NULL) { // 头指针为空,说明没有节点  ，可能有多个消费者 不可以为if
            pthread_cond_wait(&has_product, &lock); // * 阻塞等待时会暂时放弃锁，被唤醒时再加锁
        }
        // 删除节点模拟消费掉一个产品
        mp = head; // 删除节点
        head = mp->next; //模拟消费掉一个产品
        // 解锁
        pthread_mutex_unlock(&lock);

        printf("------Consume ---%d\n", mp->num);
        free(mp);
        mp = NULL;
        sleep(rand() % 5);
    }
}

void *producer(void *p) {
    for (;;) {
        // 创建节点模拟生产一个产品
        mp = malloc(sizeof(struct msg));
        mp->num = rand() % 1000 + 1; //模拟生产一个产品
        printf("++++++Produce ---%d\n", mp->num);

        // 读写共享数据（链表）
        // 加锁
        pthread_mutex_lock(&lock); // pthread_cond_wait 阻塞等待时会放弃锁，能够加锁
        mp->next = head; // 头插法 插入节点
        head = mp;
        // 解锁
        pthread_mutex_unlock(&lock);

        // * pthread_cond_signal 唤醒 pthread_cond_wait
        pthread_cond_signal(&has_product); //将等待在该条件变量上的一个线程唤醒
        sleep(rand() % 5);
    }
}

int main(int argc, char *argv[]) {
    pthread_t pid, cid;
    srand(time(NULL));

    // 生产者线程
    pthread_create(&pid, NULL, producer, NULL);
    // 消费者线程
    pthread_create(&cid, NULL, consumer, NULL);

    // 回收线程
    pthread_join(pid, NULL);
    pthread_join(cid, NULL);

    return 0;
}
