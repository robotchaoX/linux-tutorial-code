#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*自定义的信号捕捉函数*/
/*所有信号处理函数的原型，都类此，无返回值(void)，只有一个参数，表示信号编号*/
// signo 信号编号
void sig_int(int signo) { // 处理信号时再次来相同信号会被记录下来,等当前的处理完再去处理新信号,而不是立即再去处理新信号
    printf("catch signal SIGINT %d\n", signo); //单次打印
    printf("--------------sig_int slept 3 begin s\n");
    sleep(3); // 处理信号时再次来相同信号会被记录下来,而不是立即再去处理新信号
    printf("--------------sig_int slept 3 stop s\n");
}

//为Ctrl+C信号设置捕捉函数
int main(void) {

    struct sigaction act;

    act.sa_handler = sig_int; // 指定信号捕捉后的处理函数名(即注册函数)
    act.sa_flags = 0; // 默认属性   信号捕捉函数执行期间，自动屏蔽本信号

    // 不屏蔽任何信号, 清空屏蔽
    sigemptyset(&act.sa_mask); // sigemptyset 清空信号集
    // 添加屏蔽信号
    // sigaddset(&act.sa_mask, SIGQUIT);

    // 注册信号捕捉函数
    int ret = sigaction(SIGINT, &act, NULL); // 修改信号处理动作
    if (ret < 0) {
        perror("sigaction error");
        exit(1);
    }

    printf("set sigaction\n");

    while (1) { //该循环只是为了保证有足够的时间来测试函数特性
        printf("--- main slept 1 s \n");
        sleep(1);
    }

    return 0;
}
