#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int fd[2]; // [0]读端 [1]写端

    // 创建管道
    int ret = pipe(fd);
    if (ret == -1) {
        perror("pipe error:");
        exit(1);
    }

    pid_t pid;
    int i;
    // 创建2个子线程
    for (i = 0; i < 2; i++) {
        pid = fork();
        if (pid == -1) {
            perror("pipe error:"); // ls | wc -l
            exit(1);
        }
        if (pid == 0) // 子进程不再循环创建子线程
            break;
    }

    // pipe 单双工
    if (i == 0) { //兄  ls
        close(fd[0]); // 关闭fd[0]读端
        // ls 输出到 fd[1]
        dup2(fd[1], STDOUT_FILENO); // 标准输出到 pipe写端fd[1]
        execlp("ls", "ls", NULL);
    } else if (i == 1) { // 弟 wc -l
        close(fd[1]); // 关闭fd[1]写端
        // 从 fd[0]读数据 传给 wc -l
        dup2(fd[0], STDIN_FILENO); // 标准输入到 pipe读端fd[0]
        execlp("wc", "wc", "-l", NULL);
    } else if (i == 2) { //父
        close(fd[0]);
        close(fd[1]);
        // 回收子进程
        for (i = 0; i < 2; i++) {
            pid_t wpid = wait(NULL); // 父进程阻塞等待回收子进程
            if (wpid == -1) {
                perror("wait error");
            }
        }
    }

    return 0;
}
