#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// extern char **envrion;

int main(void) {
    pid_t pid;

    pid = fork();

    if (pid > 0) {
        sleep(3);
    } else if (pid == 0) { // 子进程
        // ls -l
        char *argv[] = {"ls", "-l", "-a", NULL}; // 哨兵NULL

        execv("/bin/ls", argv);
        // execvp("ls", argv); // ./a.out  aaa  bbb  ccc
        // execl("/bin/ls", "ls", "-l", NULL);
        // execlp("ls", "ls", "-l", NULL);

        perror("execlp error");
        exit(1);
    }

    return 0;
}
