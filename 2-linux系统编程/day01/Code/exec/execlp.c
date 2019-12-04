#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("========================\n");

    char *myargv[] = {"ls", "-l", "-F", "R", "-a", NULL};

    pid_t pid = fork();
    if (pid == 0) { // 子进程
        printf("------------------------\n");
        execl("/bin/ls", "ls", "-l", "-F", "-a", NULL); // 路径+程序名，没有环境变量PATH
        // exec 只有失败时才返回，一旦成功了就不再返回

        printf("------------------------\n");
        execlp("ls", "ls", "-l", "-F", "-a", NULL); // p 带环境变量路径PATH

        printf("------------------------\n");
        execv("/bin/ls", myargv); // 命令行参数 argv

        printf("------------------------\n");
        perror("execlp"); // exec成功了则永远不会执行这里
        exit(1);

    } else if (pid > 0) { // 父进程
        sleep(1);
        printf("parent\n");
    }

    return 0;
}
