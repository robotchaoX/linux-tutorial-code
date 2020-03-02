#include <stdio.h>
#include <unistd.h>

int main(void) {
    // fork创建子进程
    pid_t pid = fork(); // fork创建进程后，每个进程（父子）都有各自对fork的返回值
    if (pid == -1) { // 创建失败返回-1
        perror("fork");
        exit(1);
    } else if (pid > 0) { // 父进程 返回子进程pid > 0
        sleep(2); // 延时让父进程后死
        printf("I'm parent pid = %d, parentID = %d\n", getpid(), getppid());
    } else if (pid == 0) { // 子进程 返回0
        printf("I'm child  pid = %d, parentID=%d\n", getpid(), getppid());
        // exec
        execl("./_output", "_output", NULL);
        perror("exec");
        exit(1);
    }

    return 0;
}
