#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SERV_PORT 8000

int main(void) {
    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;
    int sockfd;
    char buf[BUFSIZ];
    char str[INET_ADDRSTRLEN];
    int i, n;

    // 1.创建一个socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    /*初始化一个地址结构*/
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; // ipv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 监听本地任意IP INADDR_ANY = 0
    serv_addr.sin_port = htons(SERV_PORT); // port

    // 2.绑定服务器地址结构
    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    // udp不需要建立连接，直接收发数据
    printf("Accepting connections ...\n");
    while (1) {
        // 3.接收客户端发送来的数据
        clie_addr_len = sizeof(clie_addr);
        n = recvfrom(sockfd, buf, BUFSIZ, 0, (struct sockaddr *)&clie_addr, &clie_addr_len);
        if (n == -1)
            perror("recvfrom error");

        printf("received from %s at PORT %d\n", inet_ntop(AF_INET, &clie_addr.sin_addr, str, sizeof(str)),
               ntohs(clie_addr.sin_port));

        // 4.处理客户端数据
        for (i = 0; i < n; i++) {
            buf[i] = toupper(buf[i]);
        }

        // 5.发送处理完数据给客户端
        n = sendto(sockfd, buf, n, 0, (struct sockaddr *)&clie_addr, sizeof(clie_addr));
        if (n == -1)
            perror("sendto error");
    }
    close(sockfd);

    return 0;
}
