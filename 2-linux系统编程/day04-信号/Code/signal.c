#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*所有信号处理函数的原型，都类此，无返回值(void)，只有一个参数，表示信号编号*/
// 信号处理函数
void do_sig(int a) {
    printf("Hi, SIGINT, how do you do !\n");
}

// sighandler_t signal(int signum, sighandler_t handler);
// 由ANSI定义,避免使用它
int main(void) {
    // 注册信号捕捉
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
