
/* 3个线程不定时 "写" 全局资源，5个线程不定时 "读" 同一全局资源 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

/*
error : unknown type name ‘pthread_rwlock_t’
pthread_rwlock_t正好就是GNU extension的一部分. 所以指定使用c99就会把GNU extension禁用,造成pthread_rwlock_t未定义的问题.
gnuXX标准 = cXX标准 + GNU extension
解决方法就是:
方法一: 把-std=c99去掉,
方法二: 或者改成 -std=gnu99, -std=gnu89, -std=gnu90 这类
*/
int counter; //全局资源

// 定义读写锁
pthread_rwlock_t rwlock;

void *th_write(void *arg) {
    int t;
    int i = (int)arg;

    while (1) {
        t = counter;
        usleep(10000);

        // 加写锁
        pthread_rwlock_wrlock(&rwlock); // 写独占
        printf("======= write thread %d: %lu: counter=%d ++counter=%d\n", i, pthread_self(), t, ++counter);
        // 解写锁
        pthread_rwlock_unlock(&rwlock);

        usleep(500000);
    }
    return NULL;
}

void *th_read(void *arg) {
    int i = (int)arg;

    while (1) {
        // 加读锁
        pthread_rwlock_rdlock(&rwlock); // 读共享
        printf("----------------------------read thread %d: %lu: %d\n", i, pthread_self(), counter);
        // 解写锁
        pthread_rwlock_unlock(&rwlock);

        usleep(90000);
    }
    return NULL;
}

// 读写锁：读共享，写独占，写优先级高
int main(void) {
    int i;

    pthread_t tid[8];

    // 初始化读写锁
    pthread_rwlock_init(&rwlock, NULL);

    // 创建3个写线程
    for (i = 0; i < 3; i++)
        pthread_create(&tid[i], NULL, th_write, (void *)i); // 3个写线程

    // 创建5个写线程
    for (i = 0; i < 5; i++)
        pthread_create(&tid[i + 3], NULL, th_read, (void *)i); // 5个读线程

    // 回收线程
    for (i = 0; i < 8; i++)
        pthread_join(tid[i], NULL); // 回收线程

    // 销毁读写锁
    pthread_rwlock_destroy(&rwlock); //释放读写琐

    return 0;
}
