#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// 获取子进程退出状态
int main(void) {
    pid_t pid, wpid;
    int status;

    pid = fork();

    if (pid == -1) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) { // 子进程
        printf("I'm process child, pid = %d\n", getpid());
#if 1
        execl("./_error", "_error", NULL); // 异常退出
        perror("execl error");
        exit(1);
#endif
        sleep(1);
        exit(10);
    } else { // 父进程
        // wpid = wait(NULL);	//阻塞等待回收子进程，传出参数NULL
        wpid = wait(&status); //传出参数，获取子进程结束参数

        if (WIFEXITED(status)) { //正常退出
            printf("I'm parent, The child process %d exit normally\n", wpid);
            printf("return value:%d\n", WEXITSTATUS(status));

        } else if (WIFSIGNALED(status)) { //异常退出
            printf("The child process exit abnormally, killed by signal %d\n", WTERMSIG(status));
            //获取信号编号
        } else {
            printf("other...\n");
        }
    }

    return 0;
}
