#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "wrap.h"

// #define MAXLINE 8192
#define SERV_PORT 8000
#define OPEN_MAX 5000

int main(int argc, char *argv[]) {
    int i, listenfd, connfd, sockfd;
    int n, num = 0;
    ssize_t nready, efd, res;
    char buf[BUFSIZ], str[INET_ADDRSTRLEN];
    socklen_t clilen;

    struct sockaddr_in cliaddr, servaddr;
    struct epoll_event tep, ep[OPEN_MAX]; // tep: epoll_ctl参数  ep[] : epoll_wait参数

    listenfd = Socket(AF_INET, SOCK_STREAM, 0); // 1.socket

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); //端口复用

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // ipv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //  INADDR_ANY 监听本地所有IP地址
    servaddr.sin_port = htons(SERV_PORT); // port

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)); // 2.bind

    Listen(listenfd, 128); // 3.listen

    // 1.1 epoll_create创建epoll模型
    efd = epoll_create(OPEN_MAX); // 创建epoll模型, efd指向红黑树根节点
    if (efd == -1) {
        perr_exit("epoll_create error");
    }

    // 1.2 epoll_ctl将lfd添加到epoll树上
    tep.events = EPOLLIN; //对应的文件描述符可以读，监听事件为"读"
    tep.data.fd = listenfd; //指定lfd的监听事件为"读"
    res = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &tep); //将lfd及对应的结构体设置到epoll树上,efd指向红黑树根节点
    if (res == -1) {
        perr_exit("epoll_ctl error");
    }

    for (;;) {
        // 1.3 epoll_wait等待所监控文件描述符上有事件的产生
        /*epoll为server阻塞监听事件, ep为struct epoll_event类型数组, OPEN_MAX为数组容量, -1表永久阻塞*/
        nready = epoll_wait(efd, ep, OPEN_MAX, -1);
        if (nready == -1) {
            perr_exit("epoll_wait error");
        }
        for (i = 0; i < nready; i++) { // 遍历所有产生的事件
            if (!(ep[i].events & EPOLLIN)) { //如果不是"读"事件, 继续循环
                continue;
            }
            // 是"读"事件
            if (ep[i].data.fd == listenfd) { //判断满足事件的fd是不是lfd，是否是建立连接请求
                clilen = sizeof(cliaddr);
                connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen); //接受链接

                printf("client IP:%s, port:%d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
                       ntohs(cliaddr.sin_port));
                printf("cfd %d---client %d\n", connfd, ++num);

                tep.events = EPOLLIN;
                tep.data.fd = connfd; //指定新建立的连接connfd的监听事件为"读"
                res = epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &tep); //将新建立的连接connfd添加到epoll树上,efd指向根节点
                if (res == -1) {
                    perr_exit("epoll_ctl error");
                }
            } else { // 事件不是lfd建立连接请求,事件为已经建立连接的客户端
                sockfd = ep[i].data.fd;
                n = Read(sockfd, buf, sizeof(buf)); //读取客户端发送数据
                if (n == 0) { //读到0,说明客户端关闭链接
                    res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL); //将该文件描述符从红黑树摘除
                    if (res == -1)
                        perr_exit("epoll_ctl error");
                    Close(sockfd); //关闭与该客户端的链接
                    printf("client[%d] closed connection\n", sockfd);
                } else if (n < 0) { //出错
                    perror("read n < 0 error: ");
                    res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
                    Close(sockfd);
                } else { // 实际读到了字节数
                    printf("received from %s at port %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
                           ntohs(cliaddr.sin_port));
                    Write(STDOUT_FILENO, buf, n); // cout

                    for (i = 0; i < n; i++) {
                        buf[i] = toupper(buf[i]); //处理客户端数据，转大写
                    }
                    // Write(STDOUT_FILENO, buf, n); // cout
                    Writen(sockfd, buf, n); //处理完数据回写给客户端
                }
            }
        }
    }
    Close(listenfd);
    Close(efd);

    return 0;
}
