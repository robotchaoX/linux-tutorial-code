#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXLINE 10
#define SERV_PORT 8000
#define OPEN_MAX 5000

// 基于网络C / S非阻塞模型的epoll ET触发模式
int main(void) {
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    int listenfd, connfd;
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];
    int efd, flag;

    listenfd = socket(AF_INET, SOCK_STREAM, 0); // 1.socket

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); //端口复用

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // ipv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //  INADDR_ANY 监听本地所有IP地址
    servaddr.sin_port = htons(SERV_PORT); // port

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)); // 2.bind

    listen(listenfd, 128); // 3.listen

    struct epoll_event event;
    struct epoll_event resevent[OPEN_MAX];
    int res, len;

    // 1.1 epoll_create创建epoll模型
    efd = epoll_create(OPEN_MAX); // 创建epoll模型, efd指向红黑树根节点
    if (efd == -1) {
        perror("epoll_create error");
        exit(-1);
    }

    // 1.2 epoll_ctl将lfd添加到epoll树上
    event.events = EPOLLIN | EPOLLET; /* ET 边沿触发，默认是水平触发 */
    // event.events = EPOLLIN;// LT 水平触发 (默认)
    event.data.fd = listenfd; //指定lfd的监听事件为"读"
    res = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &event); //将lfd及对应的结构体设置到epoll树上,efd指向红黑树根节点
    if (res == -1) {
        perror("epoll_ctl error");
        exit(-1);
    }

    while (1) {
        // 1.3 epoll_wait等待所监控文件描述符上有事件的产生
        printf("epoll_wait begin\n");
        int nready = epoll_wait(efd, resevent, OPEN_MAX, -1); //最多OPEN_MAX个, 阻塞监听
        printf("epoll_wait end res %d\n", nready);
        if (nready == -1) {
            perror("epoll_wait error");
            exit(-1);
        }

        int i;
        for (i = 0; i < nready; i++) { // 遍历所有产生的事件
            if (!(resevent[i].events & EPOLLIN)) { //如果不是"读"事件, 继续循环
                continue;
            }
            // 是"读"事件
            if (resevent[i].data.fd == listenfd) { //判断满足事件的fd是不是lfd，是否是建立连接请求
                printf("Accepting connections ...\n");
                cliaddr_len = sizeof(cliaddr);
                connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len); //接受链接
                printf("received from %s at PORT %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
                       ntohs(cliaddr.sin_port));

                flag = fcntl(connfd, F_GETFL); /* 修改connfd为非阻塞读 */
                flag |= O_NONBLOCK;
                fcntl(connfd, F_SETFL, flag);

                event.events = EPOLLIN | EPOLLET; /* ET 边沿触发，默认是水平触发 */
                // event.events = EPOLLIN;// LT 水平触发 (默认)
                event.data.fd = connfd; //指定新建立的连接connfd的监听事件为"读"
                res = epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &event); //将新建立的连接connfd添加到epoll树上,efd指向根节点
                if (res == -1) {
                    perror("epoll_ctl error");
                    exit(-1);
                }
            } else { // 事件不是lfd建立连接请求,事件为已经建立连接的客户端connfd
                int sockfd = resevent[i].data.fd;
                len = read(sockfd, buf, MAXLINE / 2); //读取客户端发送数据,//非阻塞读,只读一半
                if (len == 0) { //读到0,说明客户端关闭链接
                    res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL); //将该文件描述符从红黑树摘除
                    if (res == -1)
                        perror("epoll_ctl error");
                    close(sockfd); //关闭与该客户端的链接
                    printf("client[%d] closed connection\n", sockfd);
                } else if (len < 0) { //出错
                    perror("read n < 0 error: ");
                    res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
                    close(sockfd);
                } else { // 实际读到了字节数
                    printf("received from %s at port %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
                           ntohs(cliaddr.sin_port));

                    write(STDOUT_FILENO, buf, len); // cout
                    while ((len = read(sockfd, buf, MAXLINE / 2)) > 0) { //非阻塞读, 轮询
                        write(STDOUT_FILENO, buf, len); // cout
                    }
                }
            }
        }
    }

    return 0;
}
