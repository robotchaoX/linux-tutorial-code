#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXLINE 4096
#define CLIENT_PORT 9999 // 重要

int main(int argc, char *argv[]) {
    struct sockaddr_in localaddr;
    int confd;
    ssize_t len;
    char buf[MAXLINE];

    // 1.创建一个socket
    confd = socket(AF_INET, SOCK_DGRAM, 0);

    // 初始化本地端地址
    bzero(&localaddr, sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &localaddr.sin_addr.s_addr); // "0.0.0.0"也是INADDR_ANY局域网内所有ip
    localaddr.sin_port = htons(CLIENT_PORT);

    // 2.显式绑定本地端地址
    int ret = bind(confd, (struct sockaddr *)&localaddr, sizeof(localaddr)); //显示绑定不能省略
    if (ret == 0) {
        printf("...bind ok...\n");
    }

    while (1) {
        // 3.接收server广播
        len = recvfrom(confd, buf, sizeof(buf), 0, NULL, 0); // 接收server广播
        write(STDOUT_FILENO, buf, len); // cout
    }
    close(confd);

    return 0;
}
