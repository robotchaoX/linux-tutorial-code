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

    printf("Accepting connections ...\n");
    cliaddr_len = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
    printf("received from %s at PORT %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
           ntohs(cliaddr.sin_port));

    flag = fcntl(connfd, F_GETFL); /* 修改connfd为非阻塞读 */
    flag |= O_NONBLOCK;
    fcntl(connfd, F_SETFL, flag);

    // 1.2 epoll_ctl将lfd添加到epoll树上
    event.events = EPOLLIN | EPOLLET; /* ET 边沿触发，默认是水平触发 */
    // event.events = EPOLLIN;// LT 水平触发 (默认)
    event.data.fd = connfd;
    epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &event); //将connfd加入监听红黑树
    while (1) {
        printf("epoll_wait begin\n");
        // 1.3 epoll_wait等待所监控文件描述符上有事件的产生
        res = epoll_wait(efd, resevent, 10, -1); //最多10个, 阻塞监听
        printf("epoll_wait end res %d\n", res);

        if (resevent[0].data.fd == connfd) {
            while ((len = read(connfd, buf, MAXLINE / 2)) > 0) //非阻塞读, 轮询
                write(STDOUT_FILENO, buf, len);
        }
    }

    return 0;
}
