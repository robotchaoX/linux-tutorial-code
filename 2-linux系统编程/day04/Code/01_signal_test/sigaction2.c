/*自动屏蔽本信号，调用完毕后屏蔽自动解除*/

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

/*自定义的信号捕捉函数*/
void sig_int(int signo) { // 信号处理函数执行期间，该信号多次递送，那么只在处理函数之行结束后，处理一次。
    printf("catch signal SIGINT\n");
    sleep(3); //模拟信号处理函数执行很长时间
    printf("end of handler\n");
}

int main(void) {
    struct sigaction act, old;

    act.sa_handler = sig_int;
    act.sa_flags = 0;

    sigemptyset(&act.sa_mask); //依然不屏蔽任何信号

    sigaction(SIGINT, &act, &old); //注册信号处理函数
    printf("set sigaction\n");

    while (1)
        ;

    sigaction(SIGINT, &old, NULL); //注册信号处理函数

    return 0;
}
