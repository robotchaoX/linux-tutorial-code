#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

// 孤儿进程
// 父进程先死了,子进程没死
int main(void) {
    pid_t pid;
    pid = fork();

    if (pid == 0) { // 子进程
        while (1) {
            printf("I am child pid =%d, my parent ppid = %d\n", getpid(), getppid());
            sleep(1);
        }
    } else if (pid > 0) { // 父进程
        printf("I am parent, my pid is = %d\n", getpid());
        sleep(9);
        printf("------------parent going to die------------\n");
        // 父进程先死了,子进程还活着,被init 1 接管(孤儿进程的父进程为init 1)
        // kill 子进程pid 杀死孤儿进程
    } else { // error
        perror("fork");
        return 1;
    }

    return 0;
}
