#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void sys_err(const char *str) {
    perror(str);
    exit(1);
}

// pipe数据从写端流入 读端流出，不能反复读，
// 半双工，只能用于有公共祖先进程间
int main(void) {

    int fd[2]; // pipe读写端  [0]读端 [1]写端

    // 创建管道pipe
    int ret = pipe(fd);
    if (ret == -1) {
        // sys_err("pipe");
        perror("pipe error:");
        exit(1);
    }

    // fork子进程
    pid_t pid = fork();
    if (pid < 0) {
        // sys_err("fork err");
        perror("fork err");
        exit(1);
    } else if (pid == 0) { // 子进程 读数据
        char buf[1024]; // 读的缓存
        close(fd[1]); // 关闭fd[1]写端  // pipe 单双工 一端读，一端写
        // 读pipe
        int len = read(fd[0], buf, sizeof(buf)); // 从pipe读数据
        write(STDOUT_FILENO, buf, len); // 写到STDOUT_FILENO，显示到屏幕

        close(fd[0]); // 子进程全关闭pipe
    } else { // 父进程 写数据
        char *p_str = "test for pipe\n"; // 写的内容
        close(fd[0]); // 关闭fd[0]读端  // pipe 单双工 一端读，一端写
        // 写pipe
        write(fd[1], p_str, strlen(p_str)); // 向pipe写数据

        pid_t wpid = wait(NULL); // 父进程阻塞等待回收子进程
        if (wpid == -1) {
            perror("wait error");
        }

        close(fd[1]); // 父进程全关闭pipe
    }

    return 0;
}
