#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "wrap.h"

#define SERV_PORT 6666

// 从客户端读字符，然后将每个字符转换为大写并回送给客户端
int main(void) {
    int sfd, cfd;
    int len, i;
    char buf[BUFSIZ], clie_IP[BUFSIZ];

    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;

    // 1
    sfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_PORT);

    // 2
    Bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    // 3
    Listen(sfd, 2);

    printf("wait for client connect ...\n");

    // 4
    clie_addr_len = sizeof(clie_addr_len);
    cfd = Accept(sfd, (struct sockaddr *)&clie_addr, &clie_addr_len);
    printf("cfd = ---- %d\n", cfd);

    printf("client IP: %s  port:%d\n", inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, clie_IP, sizeof(clie_IP)),
           ntohs(clie_addr.sin_port));

    while (1) {
        // 5
        len = Read(cfd, buf, sizeof(buf));
        Write(STDOUT_FILENO, buf, len);

        for (i = 0; i < len; i++) {
            buf[i] = toupper(buf[i]);
        }

        // 6
        Write(cfd, buf, len);
    }

    Close(sfd);
    Close(cfd);

    return 0;
}
