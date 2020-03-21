/* client.c */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "wrap.h"

// #define MAXLINE 8192
#define SERV_PORT 8000
#define SERV_IP "127.0.0.1"

// 多进程并发服务器
int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr;
    char buf[BUFSIZ];
    int sockfd, n;

    sockfd = Socket(AF_INET, SOCK_STREAM, 0); // 1 socket

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // ipv4
    inet_pton(AF_INET, SERV_IP, &servaddr.sin_addr); // IP
    servaddr.sin_port = htons(SERV_PORT); // port

    Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)); // 2. connect

    while (1) {
        fgets(buf, sizeof(buf), stdin); // cin
        Write(sockfd, buf, strlen(buf));

        n = Read(sockfd, buf, sizeof(buf));
        if (n == 0) {
            printf("the other side has been closed.\n");
            break;
        } else {
            Write(STDOUT_FILENO, buf, n); // cout
        }
    }

    Close(sockfd);

    return 0;
}
