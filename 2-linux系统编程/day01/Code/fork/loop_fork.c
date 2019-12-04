#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int n = 5, i; //默认创建5个子进程

    if (argc == 2) {
        n = atoi(argv[1]);
    }

    pid_t pid;

    printf("+++ 即将fork创建子进程 +++\n");
    for (i = 0; i < n; i++) {
        // 每次循环fork创建子进程
        pid = fork(); // fork创建进程后，每个进程（父子）都有各自对fork的返回值
        if (pid > 0) { // 父进程 返回子进程pid > 0
            printf("    // for main pid 下一循环我还在 , I'm parent pid = %d, parentID = %d, i = %d\n", getpid(),
                   getppid(), i);
        } else if (pid == 0) { // 子进程 返回0
            printf("    /--/ break for !!!! , I'm new child  pid = %d, parentID=%d, i= %d\n", getpid(), getppid(), i);
            break; // ! 新创建子进程for出口,子进程break for不再参与循环fork，但是继续执行for之后的语句
        } else if (pid == -1) { // 创建失败返回-1
            perror("fork");
            exit(1);
        }
        printf("for 循环一次结束 ------\n");
    }
    if (i < n) {
        printf("#--# sleep I'm %dth child, pid = %d,sleep(%d)\n", i + 1, getpid(), i + 1);
        sleep(i + 1);
        printf("I'm %dth child, pid = %d\n", i + 1, getpid());
    } else {
        printf("## sleep I am parent, pid = %d,sleep(%d)\n", getpid(), n + 1);
        sleep(n + 1);
        printf("I am parent, pid = %d\n", getpid());
    }

    return 0;
}
