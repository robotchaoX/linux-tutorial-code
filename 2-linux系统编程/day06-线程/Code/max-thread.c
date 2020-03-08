#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *tfn(void *arg) {
    while (1)
        sleep(1);
}

// 创建的最大线程数
int main(void) {
    pthread_t tid;
    int ret, count = 1;

    for (;;) {
        ret = pthread_create(&tid, NULL, tfn, NULL);
        if (ret != 0) {
            printf("%s\n", strerror(ret));
            break;
        }
        printf("---------%d\n", ++count);
    }

    return 0;
}
