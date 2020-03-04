#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef void (*sighandler_t)(int);

// 信号处理函数
void catchsigint(int signo) {
    printf("++++++++++++++++++catch\n");
}

int main(void) {
    sighandler_t handler;

    // 注册信号捕捉
    handler = signal(SIGINT, catchsigint);
    if (handler == SIG_ERR) {
        perror("signal error");
        exit(1);
    }

    while (1) {
        printf("---------------------\n");
        sleep(1);
    }

    return 0;
}
