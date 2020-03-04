#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 打印未决信号集0-31
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

//打印所有常规信号的未决状态
int main(void) {

    sigset_t set, oldset, ped; // set屏蔽信号集 ped 未决信号集

#if 1

    // 屏蔽 Ctrl+C Ctrl+\ ,不屏蔽Ctrl+Z
    sigemptyset(&set); // 信号集清0
    sigaddset(&set, SIGINT); // 将某个信号加入信号集
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGKILL);
    sigaddset(&set, SIGSEGV);

#else

    // 屏蔽所有信号
    sigfillset(&set); // 信号集全置1

#endif

    // 设置屏蔽信号、解除屏蔽信号
    sigprocmask(SIG_BLOCK, &set, &oldset); // SIG_BLOCK:需要屏蔽的信号 //oldset 获取原屏蔽字

    while (1) {
        // 读取当前进程的未决信号集
        sigpending(&ped); //获取未决信号集
        printset(&ped);
        sleep(1);
    }

    return 0;
}
