#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int n = 0; // 计数
int flag = 0; // 通过flag变量标记程序实行调度

void sys_err(char *str) {
    perror(str);
    exit(1);
}

void do_sig_child(int num) {
    printf("I am child  %d\t%d\n", getpid(), n);
    n += 2;
    flag = 1;
    // sleep(1);// 当sleep取消，程序即会出现问题
}

void do_sig_parent(int num) {
    printf("--- I am parent %d\t%d\n", getpid(), n);
    n += 2;
    flag = 1;
    // sleep(1); // 当sleep取消，程序即会出现问题
}

// 父子进程交替数数程序。当捕捉函数里面的sleep取消，程序即会出现问题,导致无法再继续交替数数下去。
int main(void) {
    pid_t pid;
    struct sigaction act;

    if ((pid = fork()) < 0) {
        sys_err("fork");
    } else if (pid > 0) { // 父进程
        n = 1;
        sleep(1);
        act.sa_handler = do_sig_parent;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        // 自定义用户信号2
        sigaction(SIGUSR2, &act, NULL); //注册自己的信号捕捉函数   父使用SIGUSR2信号

        do_sig_parent(0); // 先开始数数

        while (1) {
            /* wait for signal */;
            if (flag == 1) { //父进程数数完成
                // 给子进程发送信号
                kill(pid, SIGUSR1); // 一下两步非原子操作,会有问题
                flag = 0; //标志已经给子进程发送完信号
            }
        }
    } else if (pid == 0) { // 子进程
        n = 2;
        act.sa_handler = do_sig_child;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        // 自定义用户信号1
        sigaction(SIGUSR1, &act, NULL);

        while (1) {
            /* wait for signal */;
            if (flag == 1) { //子进程数数完成
                // 给父进程发送信号
                kill(getppid(), SIGUSR2); // 一下两步非原子操作,会有问题
                flag = 0;
            }
        }
    }

    return 0;
}
