#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// gdb调试跟踪父子进程
int main(int argc, char *argv[]) {
    int n = 5; //默认创建5个子进程
    if (argc == 2) {
        n = atoi(argv[1]);
    }

    int i;
    for (i = 0; i < n; i++) { //出口1,父进程专用出口
        if (fork() == 0) {
            break; //出口2,子进程出口,i不自增
        }
    }

    if (n == i) {
        sleep(n);
        printf("I am parent i=%d, pid = %d\n", i, getpid());
    } else {
        sleep(i);
        printf("I'm %dth child, pid = %d\n", i, getpid());
    }

    return 0;
}
