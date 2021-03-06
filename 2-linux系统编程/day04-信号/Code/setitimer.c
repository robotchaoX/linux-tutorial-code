#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/*
 * struct itimerval  {
        struct timeval{
            it_value.tv_sec;
            it_value.tv_usec;
        } it_interval;

        struct timeval {
            it_value.tv_sec;
            it_value.tv_usec;
        } it_value;

    } it, oldit;
*/

// 实现alarm函数
unsigned int my_alarm(unsigned int sec) {
    struct itimerval it, oldit;
    int ret;

    it.it_value.tv_sec = sec; // it_value：定时的时长
    it.it_value.tv_usec = 0;
    it.it_interval.tv_sec = 0; // it_interval：设定两次定时任务之间间隔的时间
    it.it_interval.tv_usec = 0;

    // 设置定时器(闹钟)，精度微秒us，可以实现 周期 定时,计时结束发送信号
    ret = setitimer(ITIMER_REAL, &it, &oldit); // 自然定时：ITIMER_REAL
    if (ret == -1) {
        perror("setitimer");
        exit(1);
    }
    return oldit.it_value.tv_sec; // 返回闹钟剩余时间
}

// 使用setitimer函数实现alarm函数，重复计算机1秒数数程序
int main(void) {
    int i;

    // 1秒后发送信号
    my_alarm(1); // 相当于 alarm(1);

    for (i = 0;; i++)
        printf("%d\n", i);

    return 0;
}
