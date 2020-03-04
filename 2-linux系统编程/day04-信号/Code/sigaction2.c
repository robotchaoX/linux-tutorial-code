/*自动屏蔽本信号，调用完毕后屏蔽自动解除*/

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

/*自定义的信号捕捉函数*/
void sig_int(int signo) { // 信号处理函数执行期间，该信号多次递送，那么只在处理函数之行结束后，处理一次。
    printf("+++++++++catch signal SIGINT\n");
    sleep(3); //模拟信号处理函数执行很长时间
    printf("+++++++++end of handler\n");
}

// 验证在信号处理函数执行期间，该信号多次递送，那么只在处理函数之行结束后，处理一次
int main(void) {
    struct sigaction act, old;

    act.sa_handler = sig_int;
    act.sa_flags = 0;

    sigemptyset(&act.sa_mask); //依然不屏蔽任何信号

    //注册信号处理函数
    sigaction(SIGINT, &act, &old); //同时要保存旧的处理方式到old
    printf("set sigaction\n");

    while (1) { //该循环只是为了保证有足够的时间来测试函数特性
        printf("--- main slept 1 s \n");
        sleep(1);
    }

    sigaction(SIGALRM, &old, NULL); //恢复SIGALRM信号原来的处理方式
    /*因为是在实现库函数,有可能用户之前设置过SIGALRM信号的处理方式*/

    return 0;
}
