#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#define MAXLINE 10

// epoll 可以监听所有类型的文件描述符的可读可写状态 pipe
//基于管道epoll LT/ET触发模式
int main(int argc, char *argv[]) {
    int efd, i;
    int pfd[2];
    pid_t pid;
    char buf[MAXLINE], ch = 'a';

    pipe(pfd); // pipe
    pid = fork();

    if (pid == 0) { //子进程 写
        close(pfd[0]); // 关闭读端
        while (1) {
            // aaaa\n
            for (i = 0; i < MAXLINE / 2; i++) {
                buf[i] = ch;
            }
            buf[i - 1] = '\n';
            ch++; // 下一个字母
            // bbbb\n
            for (; i < MAXLINE; i++) {
                buf[i] = ch;
            }
            buf[i - 1] = '\n';
            ch++;
            // aaaa\nbbbb\n
            write(pfd[1], buf, sizeof(buf)); // 总共10个字符
            sleep(1);
        }
        close(pfd[1]);

    } else if (pid > 0) { //父进程 读
        struct epoll_event event;
        struct epoll_event resevent[10]; // epoll_wait就绪返回event
        int res, len;

        close(pfd[1]); // 关闭写端

        // 1.epoll_create创建epoll模型
        efd = epoll_create(10); // 1.创建epoll模型, efd指向红黑树根节点

        // 2.epoll_ctl将lfd添加到epoll树上
        event.events = EPOLLIN | EPOLLET; // ET 边沿触发
        // event.events = EPOLLIN; // LT 水平触发 (默认)
        event.data.fd = pfd[0]; //指定pipe的监听事件为"读"
        epoll_ctl(efd, EPOLL_CTL_ADD, pfd[0], &event); // 2.epoll_ctl

        while (1) {
            // 3.epoll_wait等待所监控文件描述符上有事件的产生
            res = epoll_wait(efd, resevent, 10, -1); // 3.epoll_wait
            printf("res %d\n", res);
            if (resevent[0].data.fd == pfd[0]) {
                len = read(pfd[0], buf, MAXLINE / 2); // 一次只读一半5个字节
                write(STDOUT_FILENO, buf, len); // cout
            }
        }

        close(pfd[0]);
        close(efd);

    } else {
        perror("fork");
        exit(-1);
    }

    return 0;
}
