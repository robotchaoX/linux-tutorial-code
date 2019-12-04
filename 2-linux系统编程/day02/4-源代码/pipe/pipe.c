#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void sys_err(const char *str) {
    perror(str);
    exit(1);
}

int main(void) {
    pid_t pid;
    char buf[1024];
    int fd[2]; // 人为规定 [0]读端 [1]写端
    char *p = "test for pipe\n";

    // 创建管道pipe
    if (pipe(fd) == -1) {
        // sys_err("pipe");
        perror("pipe");
        exit(1);
    }

    // fork子进程
    pid = fork();
    if (pid < 0) {
        // sys_err("fork err");
        perror("fork err");
        exit(1);
    } else if (pid == 0) { // 子进程 读数据
        close(fd[1]); // 关闭fd[1]写端  // pipe 单双工 一端读，一端写
        int len = read(fd[0], buf, sizeof(buf)); // 从pipe读数据
        write(STDOUT_FILENO, buf, len); // 写到STDOUT_FILENO，显示到屏幕
        close(fd[0]);
    } else { // 父进程 写数据
        close(fd[0]); // 关闭fd[0]读端  // pipe 单双工 一端读，一端写
        write(fd[1], p, strlen(p)); // 向pipe写数据
        wait(NULL); // ??
        close(fd[1]);
    }

    return 0;
}
