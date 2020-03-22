#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "wrap.h"

#define MAXLINE 80
#define SERV_PORT 8000
#define SERV_IP "127.0.0.1"

int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr;
    char buf[MAXLINE];
    int sockfd, n;

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERV_IP, &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);

    Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    while (fgets(buf, MAXLINE, stdin) != NULL) {
        Write(sockfd, buf, strlen(buf));
        n = Read(sockfd, buf, MAXLINE);

        if (n == 0) {
            printf("the other side has been closed.\n");
            break;
        } else
            Write(STDOUT_FILENO, buf, n);
    }

    Close(sockfd);

    return 0;
}
