#include <stdio.h>
#include <unistd.h>

// 测试1秒钟能数多少个数
int main(void) {
    int i;

    // 在指定seconds后，给当前进程发送14）SIGALRM信号，默认终止进程
    alarm(1); // 定时器，时间到达后产生信号，默认终止进程
    // 每个进程都有且只有唯一个定时器

    for (i = 0;; i++) // 1秒能进行多少次加法
        printf("%d\n", i);

    return 0;
}
