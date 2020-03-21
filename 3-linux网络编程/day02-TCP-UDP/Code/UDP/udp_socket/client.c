#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 8000

// udp从客户端读字符，然后将每个字符转换为大写并回送给客户端
// udp不需要建立连接，udp默认支持多客户端并发
int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr;
    int sockfd, n;
    char buf[BUFSIZ];

    // 1.创建一个socket 指定IPv4 udp
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    /*初始化一个地址结构:*/
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // IPv4协议族
    inet_pton(AF_INET, SERV_IP, &servaddr.sin_addr); // 指定IP
    servaddr.sin_port = htons(SERV_PORT); //指定端口，本地转网络字节序

    // udp不需要建立连接，直接收发数据
    while (1) {
        /*从标准输入获取数据*/
        fgets(buf, BUFSIZ, stdin); // cin
        // 2.发送数据给服务器
        n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        if (n == -1)
            perror("sendto error");

        // 3.接收服务器的数据
        n = recvfrom(sockfd, buf, BUFSIZ, 0, NULL, 0); // NULL:不关心服务器端ip+port信息
        if (n == -1)
            perror("recvfrom error");
        /*写至标准输出*/
        write(STDOUT_FILENO, buf, n); // cout
    }

    close(sockfd);

    return 0;
}
