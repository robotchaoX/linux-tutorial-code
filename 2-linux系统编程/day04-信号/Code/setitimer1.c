#include <signal.h>
#include <stdio.h>
#include <sys/time.h>

int i = 0;

// 捕捉处理函数,回调
void myfunc(int signo) {

    printf("hello world %d\n", i++);

    // raise(SIGALRM);
}

//测试it_interval、it_value这两个参数的作用
int main(void) {
    struct itimerval it, oldit;

    signal(SIGALRM, myfunc); //注册SIGALRM信号的捕捉处理函数。

    it.it_value.tv_sec = 5; // it_value：定时的时长(第一次定时的时长)
    it.it_value.tv_usec = 0;

    it.it_interval.tv_sec = 3; // it_interval：设定两次定时任务之间间隔的时间(第二次后时长间隔周期)
    it.it_interval.tv_usec = 0;

    printf("----------settimer---------\n");
    // 设置定时器(闹钟)，精度微秒us，可以实现 周期 定时,计时结束发送信号
    if (setitimer(ITIMER_REAL, &it, &oldit) == -1) { // 自然定时：ITIMER_REAL
        perror("setitimer error");
        return -1;
    }

    printf("---------------------------\n");

    while (1)
        ;

    return 0;
}
