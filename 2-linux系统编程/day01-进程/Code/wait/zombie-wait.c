#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    pid_t pid, wpid;
    pid = fork();
    int status;

    if (pid == 0) { // 子进程
        printf("---child, my parent= %d, going to sleep 10s\n", getppid());
        sleep(10);
        printf("-------------child die--------------\n");
        exit(77);
    } else if (pid > 0) { // 父进程
        while (1) { // 循环
            // while (wpid != -1) { // 循环
            printf("I am parent, pid = %d, myson = %d\n", getpid(), pid);
            // wait回收子线程
            wpid = wait(&status); // 获取子进程退出状态
            if (wpid == -1) {
                perror("wait error");
                exit(1);
            }

            if (WIFEXITED(status)) { // WIFEXITED: wait if exit end为真说明子进程正常结束
                printf("child exit with %d\n", WEXITSTATUS(status)); // WEXITSTATUS: wait exit status
            } else if (WIFSIGNALED(status)) { // WIFSIGNALED: wait if signal end 为真说明子进程被信号终止（异常）
                printf("child is killed by %d\n", WTERMSIG(status)); // WTERMSIG: wait terimale signal
            }

            sleep(1);
        }
    } else {
        perror("fork");
        return -1;
    }

    return 0;
}
