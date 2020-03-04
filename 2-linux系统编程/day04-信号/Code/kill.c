#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if 1

#define N 5

// 循环创建5个子进程，任一子进程用kill函数终止其父进程
int main(void) {
    int i; //默认创建5个子进程

    // 创建5个子进程
    for (i = 0; i < N; i++) //出口1,父进程专用出口
        if (fork() == 0)
            break; //出口2,子进程出口,i不自增

    if (i == N) { // 主父进程
        printf("++++++++++++++++I am parent++++++++++++, pid = %d\n", getpid());
        while (1) { // 等待信号
            ;
        }
    } else if (i == 3) { // 子进程
        sleep(1);
        printf(".............I am child %d, pid = %d, ppid = %d kill parent\n", i, getpid(), getppid());
        pid_t parent = getppid();
        kill(parent, SIGKILL); // 杀死父线程，9) SIGKILL：无条件终止进程。
    } else { // 子进程
        printf("-------------I am child %d, pid = %d\n", i, getpid());
        // 给当前进程自己发送指定信号(自己给自己发)
        // raise 给当前进程发送指定信号(自己给自己发)
        raise(SIGKILL); // 结束当前线程自己
        // raise(SIGSEGV); // 段错误，默认产生core文件
        // abort 给自己发送异常终止信号 6) SIGABRT 信号，终止并产生core文件
        // abort(); // abort 给自己发送异常终止信号 6) SIGABRT，并产生core文件
    }

    return 0;
}

#else

// 杀死2号子进程
int main(void) {
    int i;
    pid_t pid, q;
    for (i = 0; i < N; i++) { //默认创建5个子进程
        pid = fork();
        if (pid == 0)
            break;
        if (i == 2)
            q = pid;
    }
    if (i < 5) { // 子进程
        while (1) {
            printf("I'm child %d, getpid = %u\n", i, getpid());
            sleep(1);
        }
    } else { // 父进程
        sleep(1);
        // 给2号线程发信号
        kill(q, SIGKILL); // 给指定进程发送指定信号， 成功：0；失败：-1 设置errno
        while (1) {
            ;
        }
    }

    return 0;
}

#endif
