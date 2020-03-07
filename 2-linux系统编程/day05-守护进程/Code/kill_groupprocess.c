#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void sys_err(const char *str) {
    perror(str);
    exit(-1);
}

// 终端使用 kill -SIGKILL -进程组ID(负的) 将整个进程组内的进程全部杀死
int main(int argc, char *argv[]) {
    pid_t pid;
    int i;
    int n = 5;

    if (argc < 2) {
        printf("./a.out numchild\n");
        // exit(-1);
    } else if (argc == 2) {
        n = atoi(argv[1]);
    }

    for (i = 0; i < n; i++) {
        if ((pid = fork()) < 0)
            sys_err("fork");
        else if (pid == 0)
            break;
    }

    if (pid == 0) { /* in child */
        while (1) {
            printf("I'm child pid = %d, groupid = %d\n", getpid(), getpgrp());
            sleep(1);
        }
    }
    if (pid > 0) { /* in parent */
        pid_t cpid;
        while ((cpid = wait(NULL)) > 0)
            printf("child %d is over\n", cpid);
    }

    return 0;
}
