#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// getpid获取指定进程的进程组ID
// 如果pid = 0，那么该函数作用和getpgrp一样
int main(void) {
    pid_t pid;

    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) { // 子进程
        printf("child process PID == %d\n", getpid()); // 当前进程ID
        // printf("child Group ID == %d\n", getpgrp()); // 当前进程所在组ID
        printf("child Group ID == %d\n", getpgid(0)); // 传0表当前进程,作用和getpgrp一样。
        printf("child Group ID == %d\n", getpgid(getpid())); // 当前进程所在组ID
        printf("parent Group ID == %d\n", getpgid(getppid())); // 父进程所在组ID
        exit(0); // 子进程退出
    }

    sleep(3);
    // 只剩父进程
    printf("------------------------\n");
    printf("parent process PID == %d\n", getpid());
    printf("parent Group ID is %d\n", getpgrp());

    return 0;
}
