#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void printset(sigset_t *ped) {
    int i;
    for (i = 1; i < 32; i++) {
        if ((sigismember(ped, i) == 1)) { // sigismember 判断某个信号是否在信号集中，在集合：1；不在：0
            putchar('1');
        } else {
            putchar('0');
        }
    }
    printf("\n");
}

int main(void) {

    sigset_t set, oldset, ped; // ped 未决信号集

#if 1

    sigemptyset(&set); // 信号集清0
    sigaddset(&set, SIGINT); // 将某个信号加入信号集
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGKILL);
    sigaddset(&set, SIGSEGV);

#else

    sigfillset(&set); // 信号集全置1
    // 屏蔽所有信号

#endif

    // sigprocmask 设置屏蔽信号、解除屏蔽信号
    sigprocmask(SIG_BLOCK, &set, oldset); // SIG_BLOCK:需要屏蔽的信号 //oldset 获取原屏蔽字

    while (1) {
        // 读取当前进程的未决信号集
        sigpending(&ped); //获取未决信号集
        printset(&ped);
        sleep(1);
    }

    return 0;
}
