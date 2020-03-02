#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    pid_t pid;
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) { // 子进程
        int i;
        for (i = 3; i > 0; i--) {
            printf("This is the child\n");
            sleep(1);
        }
        exit(34);
    } else { // 父进程
        int stat_val;
        waitpid(pid, &stat_val, 0); //阻塞等待回收

        if (WIFEXITED(stat_val)) { //正常退出
            printf("Child exited with code %d\n", WEXITSTATUS(stat_val));
        } else if (WIFSIGNALED(stat_val)) { //异常退出
            printf("Child terminated abnormally, killed by signal %d\n", WTERMSIG(stat_val)); //获取信号编号
        } else {
            printf("other...\n");
        }
    }

    return 0;
}
