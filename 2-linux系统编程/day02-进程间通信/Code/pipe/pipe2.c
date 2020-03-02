#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// pipe实现 ls | wc -l
// 父进程 ls 结果通过pipe 传给 子进程 wc -l
int main(void) {

    int fd[2]; // pipe读写端  [0]读端 [1]写端

    // 创建管道pipe
    int ret = pipe(fd);
    if (ret == -1) {
        perror("pipe error:");
        exit(1);
    }

    int i;
    pid_t pid;
    for (i = 0; i < 2; i++) { // 主父进程出口
        pid = fork();
        if (pid == 0) {
            break; // 子进程出口
        }
    }

    if (i == 0) { //兄
        close(fd[0]); //写,关闭读端
        dup2(fd[1], STDOUT_FILENO);
        execlp("ls", "ls", NULL);
    } else if (i == 1) { //弟
        close(fd[1]); //读，关闭写端
        dup2(fd[0], STDIN_FILENO);
        execlp("wc", "wc", "-l", NULL);
    } else { // 父
        close(fd[0]);
        close(fd[1]);
        for (i = 0; i < 2; i++) { //两个儿子wait两次
            pid_t wpid = wait(NULL); // 父进程阻塞等待回收子进程
            if (wpid == -1) {
                perror("wait error");
            }
        }
    }

    return 0;
}
