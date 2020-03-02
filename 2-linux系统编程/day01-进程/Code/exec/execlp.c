#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("========================\n");

    char *myargv[] = {"ls", "-l", "-F", "R", "-a", NULL};

    pid_t pid = fork();
    if (pid == 0) { // 子进程
        printf("--- I'm child  pid = %d, parentID=%d\n", getpid(), getppid());

        execl("/bin/ls", "ls", "-l", "-F", "-a", NULL); // 路径+程序名，没有环境变量PATH
        // exec 只有失败时才返回，一旦成功了就不再返回

        printf("-----------exec成功则不会被执行-------------\n");
        execlp("ls", "ls", "-l", "-F", "-a", NULL); // p 带环境变量路径PATH

        printf("-----------exec成功则不会被执行-------------\n");
        execv("/bin/ls", myargv); // 命令行参数 argv

        printf("-----------exec成功则不会被执行-------------\n");
        perror("execl"); // exec成功了则永远不会执行这里
        exit(1);

    } else if (pid > 0) { // 父进程
        sleep(2); // 延时让父进程后死
        printf("--- I'm parent pid = %d, parentID = %d\n", getpid(), getppid());
    }

    return 0;
}
