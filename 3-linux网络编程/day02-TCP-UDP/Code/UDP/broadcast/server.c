#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 8000 /* 无关紧要 */
#define MAXLINE 1500

#define BROADCAST_IP "192.168.1.255" // 当前局域网段内192.168.x.255
#define CLIENT_PORT 9999 /* 重要 */

// 广播
int main(void) {
    int sockfd;
    struct sockaddr_in serveraddr, clientaddr;
    char buf[MAXLINE];

    // 1.构造用于UDP通信的套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; /* IPv4 */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); /* 监听本地任意IP INADDR_ANY = 0 */
    serveraddr.sin_port = htons(SERVER_PORT); // server port 无关紧要

    // 2. bind
    bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    // 3.赋予广播权限
    int flag = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag)); // 赋予广播权限

    /*构造发送时用到的 接收广播client 地址 IP+端口  192.168.7.255+9999 */
    bzero(&clientaddr, sizeof(clientaddr));
    clientaddr.sin_family = AF_INET; // ipv4
    inet_pton(AF_INET, BROADCAST_IP, &clientaddr.sin_addr.s_addr); // 广播IP .255
    clientaddr.sin_port = htons(CLIENT_PORT); // 接收广播client port 重要

    int i = 0;
    while (1) {
        sprintf(buf, "Drink %d glasses of water\n", i++);
        // fgets(buf, sizeof(buf), stdin);
        // 4.发送广播
        sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&clientaddr,
               sizeof(clientaddr)); // 发送给接收广播client192.168.7.255：9999
        sleep(1);
    }
    close(sockfd);
    return 0;
}
