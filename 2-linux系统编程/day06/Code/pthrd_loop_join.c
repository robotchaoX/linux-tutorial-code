#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 共享全局变量
int var = 99;

void *tfn(void *arg) {
    int i;
    i = (int)arg;

    sleep(i);
    if (i == 1) {
        var = 111;
        int *var_tmp = malloc(sizeof(int)); // 开辟到堆上，main中可用
        *var_tmp = 1001;
        printf("I'm %dth pthread, pthread_id = %lu\n var = %d  var_tmp = %d\n", i + 1, pthread_self(), var, *var_tmp);
        return (void *)var_tmp; // return 和 pthread_exit 一样，可以传回参数
        // pthread_exit((void *)var_tmp);

    } else if (i == 2) {
        var = 222;
        int *var_tmp = malloc(sizeof(int));
        *var_tmp = 2002;
        printf("I'm %dth pthread, pthread_id = %lu\n var = %d  var_tmp = %d\n", i + 1, pthread_self(), var, *var_tmp);
        // return (void *)var_tmp; // return 和 pthread_exit 一样，可以传回参数
        pthread_exit((void *)var_tmp);

    } else {
        int *var_tmp = malloc(sizeof(int));
        *var_tmp = var;
        printf("I'm %dth pthread, pthread_id = %lu\n var = %d  var_tmp = %d\n", i + 1, pthread_self(), var, *var_tmp);
        // return (void *)var_tmp; // return 和 pthread_exit 一样，可以传回参数
        pthread_exit((void *)var_tmp);
    }

    return NULL;
}

int main(void) {
    int i;
    int *ret[5]; // 指针数组

    pthread_t tid[5];
    // 循环创建子线程
    for (i = 0; i < 5; i++)
        pthread_create(&tid[i], NULL, tfn, (void *)i);

    // 循环回收子线程
    for (i = 0; i < 5; i++) {
        pthread_join(tid[i], (void **)&ret[i]); // join阻塞,接收传回参数 // &ret[i]指针的指针
        printf("----main join----%d 's ret = %d\n", i, (int)*ret[i]);
    }

    printf("I'm main pthread tid = %lu\t var = %d\n", pthread_self(), var);

    // sleep(i); // 不需要等待子线程结束
    return 0;
}
