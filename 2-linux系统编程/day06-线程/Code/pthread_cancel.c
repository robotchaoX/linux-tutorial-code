#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *tfn1(void *arg) {
    printf("thread 1 running\n");

    printf("thread 1 exiting\n");
    // return 退出线程
    return (void *)111;
}

void *tfn2(void *arg) {
    printf("thread 2 running\n");
    sleep(2);

    printf("thread 2 exiting\n");
    // pthread_exit 退出线程
    pthread_exit((void *)222);
}

void *tfn3(void *arg) {
    printf("thread 3 running\n");
    while (1) {
        // printf("thread 3: I'm going to die in 3 seconds ...\n");
        // sleep(1);
        // ! pthread_cancel 取消线程不是实时的，必须本线程到达取消点才能生效，常用的系统调用都能到达取消点
        pthread_testcancel(); //如线程中没有取消点，自己添加取消点，必须子线程自己到达取消点才能生效取消
    }
    // wile死循环
    printf("-----------------\n"); // 不会执行
    return (void *)666;
}

// 终止线程的三种方法
int main(void) {
    pthread_t tid;
    void *tret = NULL;

    // return 返回调用点
    pthread_create(&tid, NULL, tfn1, NULL); // 创建线程
    pthread_join(tid, &tret); // 阻塞回收
    printf("thread 1 exit code = %d\n", (int)tret);

    // pthread_exit 退出线程
    pthread_create(&tid, NULL, tfn2, NULL); // 创建线程
    pthread_join(tid, &tret); // 阻塞回收
    printf("thread 2 exit code = %d\n", (int)tret);

    // pthread_cancel 取消线程
    pthread_create(&tid, NULL, tfn3, NULL); // 创建线程
    sleep(3);
    // 取消线程
    pthread_cancel(tid);
    sleep(1);
    // pthread_testcancel(); // main中无法给子线程添加取消点，必须子线程自己执行到取消点才能取消
    // 回收线程
    pthread_join(tid, &tret); // 被取消线程的返回值 (void *) -1
    printf("thread 3 exit code = %d\n", (int)tret); // -1

    return 0;
}
