#include <arpa/inet.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "wrap.h"

#define SERV_ADDR "serv.socket"
#define CLIE_ADDR "clie.socket" // 客户端本地套接字地址（socket名）

int main(void) {
    int cfd, len;
    struct sockaddr_un servaddr, cliaddr;
    char buf[4096];

    // 1.创建一个socket
    cfd = Socket(AF_UNIX, SOCK_STREAM, 0);

    /* 构造 client 地址 */
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sun_family = AF_UNIX; // 本地套接字
    strcpy(cliaddr.sun_path, CLIE_ADDR); // 本地套接字地址（socket名）
    len = offsetof(struct sockaddr_un, sun_path) + strlen(cliaddr.sun_path); /* 计算客户端地址结构有效长度 */

    unlink(CLIE_ADDR); /* 确保bind之前clie.sock文件不存在,bind会创建该文件 */

    // 2.bind
    Bind(cfd, (struct sockaddr *)&cliaddr, len); /* 客户端也需要bind, 不能依赖自动绑定*/

    /* 构造 server 地址 */
    bzero(&servaddr, sizeof(servaddr)); /* 构造server 地址 */
    servaddr.sun_family = AF_UNIX; // 本地套接字
    strcpy(servaddr.sun_path, SERV_ADDR); // 本地套接字地址（socket名）
    len = offsetof(struct sockaddr_un, sun_path) + strlen(servaddr.sun_path); /* 计算服务器端地址结构有效长度 */
    // 3.根据server地址链接指定服务器进程
    Connect(cfd, (struct sockaddr *)&servaddr, len);

    while (1) {
        /*从标准输入获取数据*/
        fgets(buf, sizeof(buf), stdin); // cin
        // 4.将数据写给服务器
        write(cfd, buf, strlen(buf));
        // 5.从服务器读回转换后数据
        len = read(cfd, buf, sizeof(buf));
        /*写至标准输出*/
        write(STDOUT_FILENO, buf, len); // cout
    }

    close(cfd);

    return 0;
}
