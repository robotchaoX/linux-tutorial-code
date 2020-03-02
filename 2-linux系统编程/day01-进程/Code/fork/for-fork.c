#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    int i;
    pid_t pid; // 进程id
    printf("xxxxxxxxxxx\n");

    //创建5个子进程,所有子进程都是主父进程fork出来的
    for (i = 0; i < 5; i++) { //出口1,主父进程循环结束
        // fork创建子进程
        pid = fork(); // fork创建进程后，每个进程（父子）都有各自对fork的返回值
        if (pid == -1) { // 创建失败返回-1
            perror("fork");
            exit(1);
        } else if (pid > 0) { // 父进程 返回子进程pid > 0
            printf("--- I'm parent pid = %d, parentID = %d\n", getpid(), getppid());
        } else if (pid == 0) { // 子进程 返回0
            break; // 出口2, 子进程循环fork出口,不再继续fork孙进程, i不自增
            printf("--- I'm child  pid = %d, parentID=%d\n", getpid(), getppid());
        }
    }

    printf("+++ 父子进程都会执行 +++\n");
    sleep(i);
    if (i < 5) {
        printf("I'am %d child , pid = %u\n", i, getpid());
    } else {
        printf("I am parent i=%d, pid = %d\n", i, getpid());
    }

    return 0;
}
