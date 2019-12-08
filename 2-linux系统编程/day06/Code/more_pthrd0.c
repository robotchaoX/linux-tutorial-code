#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *tfn(void *arg) {
    int i;

    i = (int)arg;
    sleep(i); //通过i来区别每个线程
    printf("I'm %dth thread, Thread_ID = %lu\n", i + 1, pthread_self());

    return NULL;
}

int main(int argc, char *argv[]) {
    int n = 5, i;
    pthread_t tid;

    if (argc == 2)
        n = atoi(argv[1]);

    for (i = 0; i < n; i++) {
        pthread_create(&tid, NULL, tfn, (void *)i); // i局部变量，单向值传递
        //将i转换为指针，在tfn中再强转回整形。warning
    }
    sleep(n);
    printf("I am main, and I am not a process, I'm a thread!\n"
           "main_thread_ID = %lu\n",
           pthread_self());

    return 0;
}
