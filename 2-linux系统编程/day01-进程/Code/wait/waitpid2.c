#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    pid_t pid, wpid;
    int flg = 0;

    pid = fork();

    if (pid == -1) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) { // son
        printf("I'm process child, pid = %d\n", getpid());
        sleep(5);
        exit(4);
    } else { // parent
        do {
            wpid = waitpid(pid, NULL, WNOHANG); // 非阻塞等待回收
            // wpid = wait(NULL);
            printf("--- wpid = %d--------flag %d\n", wpid, flg++);
            if (wpid == 0) {
                printf("NO child exited\n");
                sleep(1);
            }
        } while (wpid == 0); //子进程仍在运行不可回收

        if (wpid == pid) { //回收了指定子进程
            printf("I'm parent, I catch child process,pid = %d\n", wpid);
        } else {
            printf("other...\n");
        }
    }

    return 0;
}
