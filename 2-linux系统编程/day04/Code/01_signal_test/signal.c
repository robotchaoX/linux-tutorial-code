#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void do_sig(int a) {
    printf("Hi, SIGINT, how do you do !\n");
}

// sighandler_t signal(int signum, sighandler_t handler);
// 由ANSI定义,避免使用它
int main(void) {
    if (signal(SIGINT, do_sig) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    while (1) {
        printf("---------------------\n");
        sleep(1);
    }

    return 0;
}
