#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 会话：一组进程组
// getsid() // 获取进程所属的会话ID
// setsid() 创建一个会话，并以自己的ID设置进程组ID，同时也是新会话的ID。
int main(void) {
    pid_t pid;

    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) { // 子进程

        printf("child process PID is %d\n", getpid());
        printf("Group ID of child is %d\n", getpgid(0));
        printf("Session ID of child is %d\n", getsid(0)); // 获取进程所属的会话ID

        sleep(10);
        // 设置新会话
        setsid(); //子进程非组长进程，故其成为新会话首进程，且成为组长进程。该进程组id即为会话进程

        printf("Changed:\n");

        printf("child process PID is %d\n", getpid());
        printf("Group ID of child is %d\n", getpgid(0));
        printf("Session ID of child is %d\n", getsid(0));

        sleep(10);

        printf("-----child byebye-----\n");
        // exit(0);
    } else { // 父进程
        // 父进程结束
        exit(0);
    }
    printf("------all byebye-----\n");
    return 0; // 父进程直接return了,shell获得
}
