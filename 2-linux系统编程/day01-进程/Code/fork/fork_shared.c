#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 父子进程间遵循 读时共享 写时复制 原则 （共享文件描述符，mmap建立的映射区）
// 全局变量不能用于进程间通信
int a = 100; //.data

int main(void) {
    pid_t pid;
    pid = fork();

    if (pid == 0) { // son
        a = 2000;
        printf("child, a = %d\n", a);
    } else {
        sleep(1); //保证son先运行
        printf("parent, a = %d\n", a);
    }

    return 0;
}
