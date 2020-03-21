#include <arpa/inet.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "wrap.h"

#define SERV_ADDR "serv.socket" // 本地套接字地址（socket名）

// 本地套接字
int main(void) {
    int lfd, cfd, len, size, i;
    struct sockaddr_un servaddr, cliaddr;
    char buf[4096];

    // 1.socket
    lfd = Socket(AF_UNIX, SOCK_STREAM, 0);

    /* 构造 server 地址 */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX; // 本地套接字
    strcpy(servaddr.sun_path, SERV_ADDR); // 本地套接字地址（socket名）

    len = offsetof(struct sockaddr_un, sun_path) + strlen(servaddr.sun_path); /* servaddr total len */

    unlink(SERV_ADDR); /* 确保bind之前serv.sock文件不存在,bind会创建该文件 */
    // 2.bind
    Bind(lfd, (struct sockaddr *)&servaddr, len); /* 参3不能是sizeof(servaddr) */

    // 3.listern
    Listen(lfd, 20);

    printf("Accept ...\n");
    while (1) {
        len = sizeof(cliaddr);
        // 4. accept
        cfd = Accept(lfd, (struct sockaddr *)&cliaddr, (socklen_t *)&len);

        len -= offsetof(struct sockaddr_un, sun_path); /* 得到文件名的长度 */
        cliaddr.sun_path[len] = '\0'; /* 添加字符串结束标记，确保打印时,没有乱码出现 */

        printf("client bind filename %s\n", cliaddr.sun_path); // socket文件名

        while ((size = read(cfd, buf, sizeof(buf))) > 0) {
            // 5.处理客户端数据
            for (i = 0; i < size; i++) {
                buf[i] = toupper(buf[i]);
            }
            // 6.处理完数据回写给客户端
            write(cfd, buf, size);
        }
        close(cfd);
    }
    close(lfd);

    return 0;
}
