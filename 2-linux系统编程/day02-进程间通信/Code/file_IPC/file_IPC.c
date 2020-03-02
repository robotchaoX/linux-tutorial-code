#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 *父子进程共享打开的文件描述符------使用文件完成进程间通信.
 */
// 借助文件进行进程间通信
int main(void) {
    int fd1, fd2;
    char buf[1024];
    char *str = "---------test for shared fd in parent child process-----\n";

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        fd1 = open("file_IPC.txt", O_RDWR | O_TRUNC | O_CREAT, 0664);
        if (fd1 < 0) {
            perror("open error");
            exit(1);
        }
        write(fd1, str, strlen(str)); // 写入文件
        printf("child wrote over...\n");

    } else {
        fd2 = open("file_IPC.txt", O_RDWR);
        if (fd2 < 0) {
            perror("open error");
            exit(1);
        }
        sleep(1); //保证子进程写入数据

        int len = read(fd2, buf, sizeof(buf)); // 从文件读取
        write(STDOUT_FILENO, buf, len);

        wait(NULL);
    }

    return 0;
}
