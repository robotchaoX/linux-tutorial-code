#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int i;
    pid_t pid;

    // 定义信号量
    sem_t *s;

    // 创建匿名映射区
    s = mmap(NULL, sizeof(sem_t) * 5, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    if (s == MAP_FAILED) {
        perror("fail to mmap");
        exit(1);
    }

    // 5个信号量
    for (i = 0; i < 5; i++)
        sem_init(&s[i], 0, 1); //信号量初值制定为1，信号量，变成了互斥锁

    // 5个子进程
    for (i = 0; i < 5; i++)
        if ((pid = fork()) == 0)
            break;

    if (i < 5) { //子进程
        int l, r;
        srand(time(NULL));

        // l左锁
        if (i == 4)
            l = 0, r = 4; // 最后一个线程反向，与其它抢同一把锁就有一把空余的锁，有一个线程就能拿到2把锁
        else
            l = i;
        // r右锁
        r = i + 1; // 右锁顺序都一致

        while (1) {
            // 锁住左边的锁
            sem_wait(&s[l]);
            if (sem_trywait(&s[r]) == 0) { // 尝试锁住右边的锁
                printf(" %c is eating\n", 'A' + i);
                sem_post(&s[r]); // 右边的锁没抢到，就放弃已经有的左锁
            }
            // 解锁左边的锁
            sem_post(&s[l]);
            sleep(rand() % 5);
        }
        exit(0);
    }

    // 回收子进程
    for (i = 0; i < 5; i++)
        wait(NULL);

    // 销毁信号量
    for (i = 0; i < 5; i++)
        sem_destroy(&s[i]);

    // 卸载内存映射区
    munmap(s, sizeof(sem_t) * 5);

    return 0;
}
