#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// 僵尸进程
// 子进程死了,父进程没有回收
// kill 父进程pid 杀死父进程,全部被系统回收
int main(void) {
    pid_t pid, wpid;
    pid = fork();

    if (pid == 0) {
        printf("---child, my parent= %d, going to sleep 10s\n", getppid());
        sleep(10);
        printf("-------------child die--------------\n");
        // 子进程先死了,成为僵尸进程
    } else if (pid > 0) {
        while (1) {
            printf("I am parent, pid = %d, myson = %d\n", getpid(), pid);
            sleep(1);
        }
        // 父进程也死时,所有资源被系统回收
    } else {
        perror("fork");
        return 1;
    }

    return 0;
}
