#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CLIENT_PORT 9999 // 重要
#define GROUP "239.0.0.2" // 组播IP

int main(int argc, char *argv[]) {
    struct sockaddr_in localaddr;
    int confd;
    ssize_t len;
    char buf[BUFSIZ];

    struct ip_mreqn group; /* 组播结构体 */

    // 1.创建一个socket
    confd = socket(AF_INET, SOCK_DGRAM, 0);

    // 初始化本地端地址
    bzero(&localaddr, sizeof(localaddr)); /* 初始化 */
    localaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &localaddr.sin_addr.s_addr); // "0.0.0.0"也是INADDR_ANY局域网内所有ip
    localaddr.sin_port = htons(CLIENT_PORT);

    // 2.显式绑定本地端地址
    bind(confd, (struct sockaddr *)&localaddr, sizeof(localaddr));

    inet_pton(AF_INET, GROUP, &group.imr_multiaddr); /* 设置组地址 */
    inet_pton(AF_INET, "0.0.0.0", &group.imr_address); /* 使用本地任意IP添加到组播组 */
    group.imr_ifindex = if_nametoindex("eth0"); /* 通过网卡名-->编号 ip ad */
    // 3.设置client 加入多播组
    setsockopt(confd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group)); /* 设置client 加入多播组 */

    while (1) {
        // 4.接收server组播
        len = recvfrom(confd, buf, sizeof(buf), 0, NULL, 0);
        write(STDOUT_FILENO, buf, len); // cout
    }
    close(confd);

    return 0;
}
