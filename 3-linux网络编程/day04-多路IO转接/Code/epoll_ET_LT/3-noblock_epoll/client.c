/* client.c */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 10
#define SERV_PORT 8000
#define SERV_IP "127.0.0.1"

int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr;
    char buf[MAXLINE];
    int sockfd, i;
    char ch = 'a';

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // socket

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERV_IP, &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)); // connect

    while (1) {
        // aaaa\n
        for (i = 0; i < MAXLINE / 2; i++)
            buf[i] = ch;
        buf[i - 1] = '\n';
        ch++; // 下一个字母
        // bbbb\n
        for (; i < MAXLINE; i++)
            buf[i] = ch;
        buf[i - 1] = '\n';
        ch++;
        // aaaa\nbbbb\n
        write(sockfd, buf, sizeof(buf)); // 总共10个字符
        sleep(1);
    }

    close(sockfd);

    return 0;
}
