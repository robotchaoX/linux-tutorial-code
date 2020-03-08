#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 线程间传递参数的结构体
typedef struct {
    char ch;
    int var;
    char str[64];
} myexit_t;

// 子线程回调函数
// 返回值 void * // 形参 void *
void *thrd_func(void *arg) {
    myexit_t *retvar = (myexit_t *)arg; // 强转参数为实际数据类型

    retvar->ch = 'a';
    retvar->var = 11;
    strcpy(retvar->str, "my thread 11");
    // 子线程修改的数据
    printf("I'm thread 1, Thread_ID = %lu,  Pid = %d，argv： ch = %c, var = %d, str = %s\n", pthread_self(), getpid(),
           retvar->ch, retvar->var, retvar->str);

    sleep(3); // 等待main中读取

    retvar->ch = 'b';
    retvar->var = 22;
    strcpy(retvar->str, "my thread 22");
    // 设置 pthread_exit 传出参数
    printf("I'm thread 1, Thread_ID = %lu,  Pid = %d，argv： ch = %c, var = %d, str = %s\n", pthread_self(), getpid(),
           retvar->ch, retvar->var, retvar->str);

    // 退出子线程
    pthread_exit((void *)retvar); // 退出时也可以传递参数
    // pthread_exit(NULL); // 退出当前线程，不传参
    // return NULL; // 返回到调用处？？
}

int main(void) {
    // 线程间传递参数的指针
    myexit_t *retval = malloc(sizeof(myexit_t)); // ! 开辟到堆上 // 共享数据

    retval->ch = '0';
    retval->var = 0;
    strcpy(retval->str, "main thread");
    // main初始数据
    printf("I'm main 0, Thread_ID = %lu,  Pid = %d，argv： ch = %c, var = %d, str = %s\n", pthread_self(), getpid(),
           retval->ch, retval->var, retval->str);

    pthread_t tid; // 线程id

    // 创建线程
    // tid 子线程id ， thrd_func回调函数 ， retval传递的参数指针(强转void *)
    int ret = pthread_create(&tid, NULL, thrd_func, (void *)retval); // 失败返回错误号
    if (ret != 0) {
        fprintf(stderr, "pthread_create error:%s\n", strerror(ret)); // strerror 错误号->标准错误信息字符串描述
        exit(1);
    }

    sleep(1); // 等待子线程修改数据
    // 子线程修改了main中的数据
    printf("I'm main 1, Thread_ID = %lu,  Pid = %d，argv： ch = %c, var = %d, str = %s\n", pthread_self(), getpid(),
           retval->ch, retval->var, retval->str);

    // 阻塞回收子线程
    pthread_join(tid, (void **)&retval); // 回收子线程时接收传出的参数
    // printf("ch = %c, var = %d, str = %s\n", retval->ch, retval->var, retval->str);
    // 显示 pthread_exit 传出的参数
    printf("I'm main 2, Thread_ID = %lu,  Pid = %d，argv： ch = %c, var = %d, str = %s\n", pthread_self(), getpid(),
           retval->ch, retval->var, retval->str);

    free(retval); // 释放共享数据内存

    // 退出主控线程
    pthread_exit((void *)0);
    printf("------------------------"); // 执行不到

    return 0; // 退出进程
}
