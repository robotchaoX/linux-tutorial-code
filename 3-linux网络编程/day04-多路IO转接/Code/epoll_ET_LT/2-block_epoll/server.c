#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXLINE 10
#define SERV_PORT 9000
#define OPEN_MAX 5000

// 基于网络C/S模型的epoll ET触发模式 readIO阻塞模式，不好
int main(void) {
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    int listenfd, connfd;
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];
    int efd;

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
    struct epoll_event resevent[10];
    int res, len;

    // 1.1 epoll_create创建epoll模型
    efd = epoll_create(OPEN_MAX); // 创建epoll模型, efd指向红黑树根节点
    if (efd == -1) {
        perror("epoll_create error");
        exit(-1);
    }

    event.events = EPOLLIN | EPOLLET; /* ET 边沿触发 */
    // event.events = EPOLLIN;                 /* 默认 LT 水平触发 */

    printf("Accepting connections ...\n");

    cliaddr_len = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
    printf("received from %s at PORT %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
           ntohs(cliaddr.sin_port));

    event.data.fd = connfd;
    epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &event);

    while (1) {
        res = epoll_wait(efd, resevent, 10, -1);

        printf("res %d\n", res);
        if (resevent[0].data.fd == connfd) {
            len = read(connfd, buf, MAXLINE / 2); // readn(500)
            write(STDOUT_FILENO, buf, len);
        }
    }

    return 0;
}
