#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int n = 5, i; //默认创建5个子进程
    pid_t p, q;

    if (argc == 2) {
        n = atoi(argv[1]);
    }

    for (i = 0; i < n; i++) { //出口1,父进程专用出口
        p = fork();
        if (p == 0) {
            break; //出口2,子进程出口,i不自增
        } else if (i == 3) {
            q = p;
        }
    }
    pid_t pid;
    if (n == i) { // 父进程
        sleep(n);
        printf("I am parent, pid = %d\n", getpid(), getgid());

        // pid = wait(NULL); // wait一次只回收一个子进程,结束了未回收的成为僵尸进程
        while (pid = wait(NULL)) // 阻塞循环回收子线程
            ;
        //   pid = waitpid(q, NULL, 0); // 回收指定进程，0阻塞
        //   pid = waitpid(q, NULL, WNOHANG); // 回收指定进程，WNOHANG非阻塞
        // while (pid = waitpid(-1, NULL, 0)) // 等价wait(NULL)，-1回收任意进程，0阻塞
        //     ;
        // do {
        //     pid = waitpid(-1, NULL, WNOHANG); // -1回收任意进程，WNOHANG非阻塞
        //     if (pid > 0) {
        //         n--;
        //     }     // if pid = 0 说明子进程正在运行
        // } while (n > 0);
        printf("child pid = %d\n", pid);
        while (1) {
        }

    } else { // 子进程
        sleep(i);
        printf("I'm %dth child, pid = %d, gpid=%d\n", i + 1, getpid(), getgid());
        // while (1)
        //     ;
    }

    return 0;
}
