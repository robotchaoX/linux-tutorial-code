#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void sys_err(char *str) {
    perror(str);
    exit(1);
}

// SIGCHLD 信号 默认处理动作：忽略
// 子进程结束自动发送 SIGCHLD 信号
void do_sig_child(int signo) {
    int status;
    pid_t pid;

    //    if ((pid = waitpid(0, &status, WNOHANG)) > 0) {
    while ((pid = waitpid(0, &status, WNOHANG)) > 0) { // 回收子进程
        if (WIFEXITED(status))
            printf("------------child %d exit %d\n", pid, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("child %d cancel signal %d\n", pid, WTERMSIG(status));
    }
}

int main(void) {
    pid_t pid;
    int i;
    //阻塞SIGCHLD
    // 创建10个子进程
    for (i = 0; i < 10; i++) {
        if ((pid = fork()) == 0)
            break;
        else if (pid < 0)
            sys_err("fork");
    }

    if (pid == 0) { // 10个子进程
        int n = 1;
        while (n--) {
            printf("child %d ID %d\n", i, getpid());
            sleep(1);
            // 子进程结束自动发送SIGCHLD信号
        }
        return i + 1;
    } else if (pid > 0) { // 父进程
        // SIGCHLD阻塞
        struct sigaction act;

        act.sa_handler = do_sig_child;
        act.sa_flags = 0;

        sigemptyset(&act.sa_mask);

        // 捕获子进程结束发送的 SIGCHLD 信号
        sigaction(SIGCHLD, &act, NULL);
        //解除对SIGCHLD的阻塞

        while (1) {
            printf("+++++ I'm Parent ID %d\n", getpid()); // 打印次数？？
            sleep(10); // ?? 在sleep时响应信号，响应信号后不再继续sleep,接着从sleep下一句开始执行
            printf(".....\n");
        }
    }

    return 0;
}
