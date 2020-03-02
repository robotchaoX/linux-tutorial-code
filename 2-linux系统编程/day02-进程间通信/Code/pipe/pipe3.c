#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

//是否允许有一个读端多个写端呢？
int main(void) {
    pid_t pid;
    int fd[2], i, n;
    char buf[1024];

    int ret = pipe(fd);
    if (ret == -1) {
        perror("pipe error");
        exit(1);
    }

    for (i = 0; i < 2; i++) {
        if ((pid = fork()) == 0)
            break;
        else if (pid == -1) {
            perror("pipe error");
            exit(1);
        }
    }

    if (i == 0) { // 兄
        close(fd[0]);
        write(fd[1], "1.hello\n", strlen("1.hello\n"));
        close(fd[1]);
    } else if (i == 1) { // 弟
        close(fd[0]);
        write(fd[1], "2.world\n", strlen("2.world\n"));
        close(fd[1]);
    } else { // 父
        close(fd[1]); //父进程关闭写端,留读端读取数据
        // sleep(1);
        for (int i = 0; i < 2; i++) {
            n = read(fd[0], buf, 1024); //从管道中读数据
            write(STDOUT_FILENO, buf, n);
        }

        for (i = 0; i < 2; i++) //两个儿子wait两次
            wait(NULL);
        close(fd[0]);
    }

    return 0;
}
