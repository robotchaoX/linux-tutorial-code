#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int n = 0, flag = 0; // 通过flag变量标记程序实行调度

void sys_err(char *str) {
    perror(str);
    exit(1);
}

void do_sig_child(int num) {
    printf("I am child  %d\t%d\n", getpid(), n);
    n += 2;
    flag = 1;
    sleep(1);
}

void do_sig_parent(int num) {
    printf("--- I am parent %d\t%d\n", getpid(), n);
    n += 2;
    flag = 1;
    sleep(1);
}

int main(void) {
    pid_t pid;
    struct sigaction act;

    if ((pid = fork()) < 0)
        sys_err("fork");

    else if (pid > 0) { // 父进程
        n = 1;
        sleep(1);
        act.sa_handler = do_sig_parent;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        // 用户信号
        sigaction(SIGUSR2, &act, NULL); //注册自己的信号捕捉函数   父使用SIGUSR2信号

        do_sig_parent(0); // 先开始

        while (1) {
            /* wait for signal */;
            if (flag == 1) { //父进程数数完成
                // 发送信号
                kill(pid, SIGUSR1);
                flag = 0; //标志已经给子进程发送完信号
            }
        }

    } else if (pid == 0) { // 子进程
        n = 2;
        act.sa_handler = do_sig_child;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGUSR1, &act, NULL);

        while (1) {
            /* wait for signal */;
            if (flag == 1) {
                // 发送信号
                kill(getppid(), SIGUSR2);
                flag = 0;
            }
        }
    }

    return 0;
}
