#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*自定义的信号捕捉函数*/
// signo 信号编号
void sig_int(int signo) { // 处理信号时再次来信号会被记录下来
    printf("catch signal SIGINT %d\n", signo); //单次打印
    printf("--------------sig_int slept 3 begin s\n");
    sleep(3);
    printf("--------------sig_int slept 3 stop s\n");
}

int main(void) {

    int ret;

    struct sigaction act;

    act.sa_handler = sig_int; // 指定信号捕捉后的处理函数名(即注册函数)
    act.sa_flags = 0; // 默认属性   信号捕捉函数执行期间，自动屏蔽本信号

    // 不屏蔽任何信号, 清空屏蔽
    sigemptyset(&act.sa_mask); // sigemptyset 清空信号集
    // sigaddset(&act.sa_mask, SIGQUIT);

    ret = sigaction(SIGINT, &act, NULL); // 修改信号处理动作
    if (ret < 0) {
        perror("sigaction error");
        exit(1);
    }

    printf("set sigaction\n");

    // printf("--- main slept 3 begin \n");
    // sleep(3);
    // printf("--- main slept 3 stop \n");

    while (1)
        ; //该循环只是为了保证有足够的时间来测试函数特性

    return 0;
}
