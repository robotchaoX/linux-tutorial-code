#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    pid_t pid;
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid > 0) { // 父进程
        sleep(2); // 延时让父进程后死
        printf("I'm parent pid = %d, parentID = %d\n", getpid(), getppid());
    } else if (pid == 0) { // 子进程
        sleep(1);
        printf("I'm child  pid = %d, parentID=%d\n", getpid(), getppid());
        execl("/bin/ls", "ls", "-l", NULL);
        perror("exec");
        exit(1);
    }

    printf("-------finish...%d\n", getpid());

    return 0;
}
