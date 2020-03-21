#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "wrap.h"

#define SERV_IP "127.0.0.1"
#define SERV_PORT 6666

// 从客户端读字符，然后将每个字符转换为大写并回送给客户端
int main(void) {
    int sfd, len;
    struct sockaddr_in serv_addr;
    char buf[BUFSIZ];
    // 1
    sfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(SERV_PORT);
    // 2
    Connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    while (1) {
        fgets(buf, sizeof(buf), stdin);
        // 3
        len = Write(sfd, buf, strlen(buf));
        printf("Write len ======== %d\n", len);

        // 4
        len = Read(sfd, buf, sizeof(buf));
        printf("Read  len ======== %d\n", len);
        Write(STDOUT_FILENO, buf, len);
    }

    Close(sfd);

    return 0;
}
