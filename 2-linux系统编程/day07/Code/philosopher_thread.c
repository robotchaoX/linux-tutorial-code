#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t m[5];

void *tfn(void *arg) {
    int i;
    int l, r; // l左锁 r右锁

    srand(time(NULL));
    i = (int)arg;

    // 左锁
    if (i == 4)
        l = 0, r = i; // 最后一个线程反向，与其它抢同一把锁就有一把空余的锁，有一个线程就能拿到2把锁
    else
        l = i;

    // 右锁
    r = i + 1; // 右锁顺序都一致

    while (1) {
        // 锁住左边的锁
        pthread_mutex_lock(&m[l]);

        if (pthread_mutex_trylock(&m[r]) == 0) { // 尝试锁住右边的锁
            // 左锁 右锁 都抢到，开吃
            printf("\t%c is eating \n", 'A' + i);
            pthread_mutex_unlock(&m[r]);
        }
        // 解锁左边的锁
        pthread_mutex_unlock(&m[l]); // 右边的锁没抢到，就放弃已经有的左锁
        sleep(rand() % 5);
    }

    return NULL;
}

int main(void) {
    int i;
    pthread_t tid[5];

    // 初始化5把锁
    for (i = 0; i < 5; i++)
        pthread_mutex_init(&m[i], NULL);

    // 创建5个线程
    for (i = 0; i < 5; i++)
        pthread_create(&tid[i], NULL, tfn, (void *)i);

    // 回收5个线程
    for (i = 0; i < 5; i++)
        pthread_join(tid[i], NULL);

    // 销毁5把锁
    for (i = 0; i < 5; i++)
        pthread_mutex_destroy(&m[i]);

    return 0;
}
