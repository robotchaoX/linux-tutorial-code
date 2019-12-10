#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

// Daemon(精灵)进程，是Linux中的后台服务进程，通常独立于控制终端并且周期性地执行某种任务或等待处理某些发生的事件。一般采用以d结尾的名字。
// Linux后台的一些系统服务进程，没有控制终端，不能直接和用户交互。不受用户登录、注销的影响，一直在运行着，他们都是守护进程。

// 创建守护进程
void daemonize(void) {
    pid_t pid;

    // 1.创建子进程，父进程退出
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid != 0) /* parent */ {
        exit(0); // 退出父进程
    }

    // 2.子进程成为一个新会话的首进程，失去控制终端
    setsid(); // 创建会话，使子进程完全独立出来，脱离控制

    // 3.改变当前目录为根目录
    if (chdir("/") < 0) {
        perror("chdir");
        exit(1);
    }

    // 4.重设文件权限掩码
    umask(0); // 设置umask为0  文件权限掩码

    // 5.关闭文件描述符
    // 重定向默认打开的 0，1，2文件描述符到 /dev/null，因为已经失去控制终端，不会用到，再操作0，1，2没有意义.
    close(STDIN_FILENO); // 关闭stdin 0
    open("/dev/null", O_RDWR); // null->0
    dup2(0, STDOUT_FILENO); // 1
    dup2(0, STDERR_FILENO); // 2
}

// 每次运行都会运行一个守护进程，在后台
int main(void) {
    daemonize();
    while (1) { //
        // sleep(1);
        // 6.开始执行守护进程核心工作
        ; /* 在此循环中可以实现守护进程的核心工作 */
    }

    // todo
    // 7.守护进程退出处理程序

    return 0;
}
