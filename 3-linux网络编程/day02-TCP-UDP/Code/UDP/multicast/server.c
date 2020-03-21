#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SERVER_PORT 8000 /* 无关紧要 */
#define CLIENT_PORT 9999 /* 重要 */
#define MAXLINE 1500

#define GROUP "239.0.0.2" // 组播IP

// 组播（多播）
int main(void) {
    int sockfd;
    struct sockaddr_in serveraddr, clientaddr;
    char buf[MAXLINE] = "itcast\n";
    struct ip_mreqn group;

    // 1. 构造用于UDP通信的套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); /* 构造用于UDP通信的套接字 */

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; /* IPv4 */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); /* 本地任意IP INADDR_ANY = 0 */
    serveraddr.sin_port = htons(SERVER_PORT); // server port 无关紧要

    // 2. bind
    bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    inet_pton(AF_INET, GROUP, &group.imr_multiaddr); /* 设置组地址 */
    inet_pton(AF_INET, "0.0.0.0", &group.imr_address); /* 本地任意IP */
    group.imr_ifindex = if_nametoindex("eth0"); /* 给出网卡名,转换为对应编号: eth0 --> 编号  命令:ip ad */

    // 3.赋予组播权限
    setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &group, sizeof(group)); /* 组播权限 */

    /*构造发送时用到的 接收组播client 地址 IP+端口   239.0.0.2：9999 */
    bzero(&clientaddr, sizeof(clientaddr)); /* 构造 client 地址 IP+端口 */
    clientaddr.sin_family = AF_INET; // ipv4
    inet_pton(AF_INET, GROUP, &clientaddr.sin_addr.s_addr); /* IPv4  239.0.0.2+9999 */
    clientaddr.sin_port = htons(CLIENT_PORT); // 接收组播client port 重要

    int i = 0;
    while (1) {
        sprintf(buf, "itcast %d\n", i++);
        // fgets(buf, sizeof(buf), stdin);
        // 4.发送组播
        sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&clientaddr,
               sizeof(clientaddr)); // 发送给接收组播client 239.0.0.2：9999
        sleep(1);
    }

    close(sockfd);

    return 0;
}
