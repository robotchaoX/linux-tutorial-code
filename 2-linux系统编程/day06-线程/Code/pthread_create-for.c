#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int func(int a) {
    printf("   func(%d) ---, Thread_ID = %lu\n", a, pthread_self());
    pthread_exit(NULL); // 退出当前线程
    // return 0; // 返回到调用处
    // exit(1); // 退出进程
}

// 子线程回调函数
void *tfn(void *arg) {

    int i = (int)arg; // void * -> int 强转 // 这是值传递，传递的是数值
    // i = *(int *)arg; // 错误，main地址的i值可能已经修改 void * -> int 强转 // 这是值传递，传递的是i地址
    // i = *((int *)arg); // void * -> int * 强转 // 得到共享数据的值
    sleep(i); //通过i来区别每个线程
    printf("I'm %dth thread, Thread_ID = %lu\n", i + 1, pthread_self());
    if (i == 2)
        func(8888); // 走了

    pthread_exit(NULL); // 退出当前线程
    // return NULL; // 返回到调用处？？
}

int main(int argc, char *argv[]) {
    int n = 5, i; // 局部变量，单向，值传递

    if (argc == 2)
        n = atoi(argv[1]);

    // 错误，main地址的i值可能已经修改，必须加锁
    // int *i_ptr = malloc(sizeof(int)); // 开辟到堆上，线程间共享数据

    pthread_t tid[5];
    // 创建线程
    for (i = 0; i < n; i++) { // for创建线程时5个线程不是按顺序产生的??
        pthread_create(&tid[i], NULL, tfn, (void *)i); // int -> void * 强转 // 单向值传递，传递的是数值, i局部变量
        //将i转换为指针，在tfn中再强转回整形,值01->地址01->值01(int i值当做地址)。warning
        // pthread_create(&tid, NULL, tfn, (void *)&i); // 错误，main地址的i值可能已经修改 int -> void * 强转 //
        // 这是值传递，传递的是i地址, i局部变量
        // (*i_ptr) = i;
        // pthread_create(&tid, NULL, tfn, (void *)i_ptr); // int * -> void * 强转 // 值传递，传递的是数值，传递共享数据
        // printf("i = %d , *i_ptr = %d \n", i, *i_ptr);
        // usleep(2000);
    }
    // sleep(n);
    sleep(2);
    printf("I am main, and I am not a process, I'm a thread!   main_thread_ID = %lu\n", pthread_self());

    printf("------------ main thread exit -------------\n");
    // ! 退出主线程，但是当前进程不会退出，所以其它子线程仍然可以继续执行
    pthread_exit(NULL);
    // 主线程退出，主线程下面的不会再执行
    printf("--------------------------------------"); // 不会执行

    return 0; // 退出当前进程 // 不会执行
    // exit(1); // 退出当前进程
}
