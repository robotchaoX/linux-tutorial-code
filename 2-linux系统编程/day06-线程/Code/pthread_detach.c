#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *tfn(void *arg) {
    int n = 3;

    while (n--) {
        printf("I am thread count %d\n", n);
        sleep(1);
    }

    // return (void *)1;
    pthread_exit((void *)1);
}

int main(void) {
    pthread_t tid;
    void *tret;
    int err;

// #if 0 注销  ， #if 1 有效
#if 0 

    /*通过线程属性来设置游离态*/
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid, &attr, tfn, NULL);

#else

    /*创建线程，然后分离线程*/
    pthread_create(&tid, NULL, tfn, NULL);
    pthread_detach(tid); // 让线程分离  ----无法join,自动退出,无系统残留资源

#endif

    while (1) {
        // join 阻塞,对无效pthread_t则不会阻塞等待.
        err = pthread_join(tid, &tret); // detach 后无法 join 回收,tid为无效pid,不会阻塞等待
        // printf("--------------------err = %d\n", err);
        if (err != 0)
            fprintf(stderr, "thread_join error: %s\n", strerror(err));
        else
            fprintf(stderr, "thread exit code : return %d\n", (int)tret);

        sleep(1);
    }

    return 0;
}
