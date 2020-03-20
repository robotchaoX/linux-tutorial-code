#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#if 1

int var = 1, num = 5;
pthread_mutex_t mux1_var, mux2_num; // 两把互斥锁

void *tfn(void *arg) {
    int i = (int)arg;

    if (i == 1) { // 线程1
        // 线程1 加锁 mux1_var
        pthread_mutex_lock(&mux1_var);
        printf("----I'm thread %d   lock : mux1_var \n", i);
        var = 22;
        sleep(1); //给另外一个线程加锁,创造机会.

        // 线程1 阻塞等待加锁 mux2_num ，但是锁在线程2手里，造成互锁死锁
        pthread_mutex_lock(&mux2_num); // 死锁了
        printf("----I'm thread %d   lock : mux2_num \n", i);
        num = 66;

        pthread_mutex_unlock(&mux1_var);
        printf("----I'm thread %d   unlock : mux1_var \n", i);
        pthread_mutex_unlock(&mux2_num);
        printf("----I'm thread %d   unlock : mux2_num \n", i);

        printf("----thread %d finish\n", i);
        pthread_exit(NULL);

    } else if (i == 2) { // 线程2
        // 线程2 加锁 mux2_num
        pthread_mutex_lock(&mux2_num);
        printf("----I'm thread %d   lock : mux2_num \n", i);
        var = 33;
        sleep(1);

        // 线程1 阻塞等待加锁 mux1_var，但是锁在线程2手里，造成互锁死锁
        pthread_mutex_lock(&mux1_var); // 死锁了
        printf("----I'm thread %d   lock : mux1_var \n", i);
        num = 99;

        pthread_mutex_unlock(&mux1_var);
        printf("----I'm thread %d   unlock : mux1_var \n", i);
        pthread_mutex_unlock(&mux2_num);
        printf("----I'm thread %d   unlock : mux2_num \n", i);

        printf("----thread %d finish\n", i);
        pthread_exit(NULL);
    }

    return NULL;
}

// 互锁
int main(void) {
    // 初始化两把锁
    pthread_mutex_init(&mux1_var, NULL);
    pthread_mutex_init(&mux2_num, NULL);

    pthread_t tid1, tid2;
    // 创建两个线程
    pthread_create(&tid1, NULL, tfn, (void *)1);
    pthread_create(&tid2, NULL, tfn, (void *)2);

    sleep(3); // 给子线程执行时间
    printf("++++++++++++var = %d, num = %d++++++++++++\n", var, num);

    int ret1 = pthread_mutex_destroy(&mux1_var); //释放琐
    int ret2 = pthread_mutex_destroy(&mux2_num);
    if (ret1 == 0 && ret2 == 0)
        printf("++++++++++++destroy mutex finish\n");
    else
        printf("++++++++++++destroy mutex not finish++++++++++++++\n");

    // 阻塞等待回收线程
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("++++++++++++join thread finish\n");

    return 0;
}

#else

int a = 100;

// 自锁
int main(void) {
    // 静态初始化mutex
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&mutex);
    printf("-----------pthread_mutex_lock-----once----\n");
    a = 777;
    // ! 同一把锁多次加锁，造成死锁
    pthread_mutex_lock(&mutex); // 阻塞等待加锁
    printf("-----------pthread_mutex_lock-----second----\n");

    pthread_mutex_unlock(&mutex);
    printf("-----------pthread_mutex_unlock---------\n");

    printf("-----------a = %d\n", a);
    pthread_mutex_destroy(&mutex);

    return 0;
}

#endif
