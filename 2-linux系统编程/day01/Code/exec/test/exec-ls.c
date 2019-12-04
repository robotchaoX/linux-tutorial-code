#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    pid_t pid;

    pid = fork();

    if (pid > 0) {
        sleep(3);

    } else if (pid == 0) {
        // ls -l
        char *argv[] = {"ls", "-l", "-a", NULL}; // 哨兵NULL

        execvp("ls", argv); // ./a.out  aaa  bbb  ccc
        perror("execlp error");
        exit(1);
    }

    return 0;
}
