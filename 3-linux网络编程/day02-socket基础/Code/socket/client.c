#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 6666

// 从客户端读字符，然后将每个字符转换为大写并回送给客户端
int main(void) {
    int sfd, len;
    struct sockaddr_in serv_addr;
    char buf[BUFSIZ];

    /*创建一个socket 指定IPv4 TCP*/
    sfd = socket(AF_INET, SOCK_STREAM, 0);

    /*初始化一个地址结构:*/
    bzero(&serv_addr, sizeof(serv_addr)); //清零
    serv_addr.sin_family = AF_INET; // IPv4协议族
    // inet_pton IP地址转换函数,字符串类型转换为网络字节序,参3:传出参数
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr); // 指定IP
    // serv_addr.sin_addr.s_addr = htonl(SERV_IP); // 指定IP地址??
    serv_addr.sin_port = htons(SERV_PORT); //指定端口 本地转网络字节序

    /*根据地址结构链接指定服务器进程*/
    connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    while (1) {
        /*从标准输入获取数据*/
        fgets(buf, sizeof(buf), stdin); // cin
        /*将数据写给服务器*/
        write(sfd, buf, strlen(buf)); //写个服务器

        /*从服务器读回转换后数据*/
        len = read(sfd, buf, sizeof(buf));
        /*写至标准输出*/
        write(STDOUT_FILENO, buf, len); // cout
    }

    /*关闭链接*/
    close(sfd);

    return 0;
}
