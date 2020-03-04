/*当执行SIGINT信号处理函数期间
 *多次收到SIGQUIT信号都将被屏蔽(阻塞)
 *SIGINT信号处理函数处理完，立刻解除对
 *SIGQUIT信号的屏蔽，由于没有捕捉该信号，
 *将立刻执行该信号的默认动作，程序退出
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void sig_int(int signo) {
    printf("+++++++++catch signal SIGINT\n");
    sleep(3); //模拟信号处理函数执行很长时间
    printf("+++++++++end of handler\n");
}

// 验证sa_mask在捕捉函数执行期间的屏蔽作用
int main(void) {
    struct sigaction act;

    act.sa_handler = sig_int; // 指定信号捕捉后的处理函数名(即注册函数)
    act.sa_flags = 0; // 通常设置为0，表使用默认属性

    sigemptyset(&act.sa_mask); // 清空信号屏蔽集//不屏蔽任何信号
    sigaddset(&act.sa_mask, SIGQUIT); // 将某个信号加入信号屏蔽集
    /*将SIGQUIT加入信号屏蔽集,这就导致,
     *在调用信号处理函数期间
     *不仅不响应SIGINT信号本身(相同信号),还不响应SIGQUIT(被屏蔽了)*/

    //注册信号SIGINT捕捉函数
    sigaction(SIGINT, &act, NULL);
    printf("set sigaction\n");

    while (1)
        ;

    return 0;
}
