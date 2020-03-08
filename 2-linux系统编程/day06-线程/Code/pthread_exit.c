#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *tfn(void *arg) {
    int i;

    //通过i来区别每个线程
    i = (int)arg; //强转

    sleep(i); //通过i来区别每个线程

    if (i == 2) {
        printf("------- i==2 pthread_exit-------\n");
        pthread_exit(NULL); // 退出一个子线程,不传参数
        // pthread_exit((void *)ret); // 子线程退出时传出参数 void *
        // return (void *)ret; // return 和 pthread_exit 一样，可以传回参数
    }

    printf("I'm %dth thread, Thread_ID = %lu\n", i + 1, pthread_self());

    return NULL;
}

int main(int argc, char *argv[]) {
    int n = 5, i;

    if (argc == 2)
        n = atoi(argv[1]);

    pthread_t tid;
    for (i = 0; i < n; i++) {
        pthread_create(&tid, NULL, tfn, (void *)i); // 传递参数为 void * 类型
        //将i转换为指针，在tfn中再强转回整形。
    }

    sleep(n);
    printf("I am main, I'm a thread!\n"
           "main_thread_ID = %lu\n",
           pthread_self());

    return 0;
}
