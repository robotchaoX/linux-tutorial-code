#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// getppid() 获取当前进程的进程组ID
// setpgid() 改变进程默认所属的进程组
int main(void) {
    pid_t pid;

    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) { // 子进程
        printf("child PID == %d\n", getpid());
        printf("child Group ID == %d\n", getpgid(0)); // 返回组id
        // printf("child Group ID == %d\n",getpgrp()); // 返回组id
        sleep(7);
        printf("----Group ID of child is changed to %d\n", getpgid(0)); // 父进程修改了子进程组ID
        exit(0);

    } else if (pid > 0) { // 父进程
        sleep(1);
        // 设置子进程组ID，进程， 进程组
        setpgid(pid, pid); //让子进程自立门户,成为进程组组长,以它的pid为进程组id

        sleep(13);
        printf("-------------------parent----------------------\n");
        printf("parent PID == %d\n", getpid());
        printf("parent Group ID == %d\n", getpgid(0));
        printf("parent's parent process PID == %d\n", getppid());

        sleep(5);
        // 设置进程组ID，进程， 进程组
        setpgid(getpid(), getppid()); // 改变父进程的组id为父进程的父进程
        printf("--------------------parent changed---------------------\n");
        printf("----Group ID of parent is changed to %d\n", getpgid(0));

        while (1)
            ;
    }

    return 0;
}
