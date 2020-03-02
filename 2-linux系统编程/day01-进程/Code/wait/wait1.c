#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    pid_t pid, wpid;

    pid = fork();

    if (pid == -1) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) { // 子进程
        printf("I'm child, pid = %d\n", getpid());
        sleep(7); // 子进程等一会
    } else { // 父进程
    lable:
        wpid = wait(NULL); // 父进程阻塞等待回收子进程
        if (wpid == -1) {
            perror("wait error");
            goto lable;
        }
        printf("I'm parent, I couch child process,pid = %d\n", wpid);
    }

    return 0;
}
