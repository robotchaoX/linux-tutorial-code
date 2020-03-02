#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 父子进程间遵循 读时共享 写时复制 原则 （共享文件描述符，mmap建立的映射区）
// 全局变量不能用于进程间通信
int var = 34;

int main(void) {
    pid_t pid;

    printf("+++ 即将fork创建子进程 +++\n");
    // fork创建子进程
    pid = fork(); // fork创建进程后，每个进程（父子）都有各自对fork的返回值
    if (pid == -1) { // 创建失败返回-1
        perror("fork");
        exit(1);
    } else if (pid > 0) { // 父进程 返回子进程pid > 0
        sleep(2); // 延时让父进程后死
        var = 55;
        printf("I'm parent pid = %d, parentID = %d, var = %d\n", getpid(), getppid(), var);
    } else if (pid == 0) { // 子进程 返回0
        var = 100;
        printf("I'm child  pid = %d, parentID=%d, var = %d\n", getpid(), getppid(), var);
    }
    printf("--- 父进程 子进程都会执行 ---\n");
    printf("var = %d\n", var);

    return 0;
}
