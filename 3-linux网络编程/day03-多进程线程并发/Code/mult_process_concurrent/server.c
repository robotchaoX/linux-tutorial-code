#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "wrap.h"

// #define MAXLINE 8192
#define SERV_PORT 8000

// 回收子进程
void do_sigchild(int num) {
    while (waitpid(0, NULL, WNOHANG) > 0)
        ;
}

// 多进程并发服务器
// 每次建立新连接，新建一个进程处理
// 从客户端读字符，然后将每个字符转换为大写并回送给客户端
int main(void) {
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    int listenfd, connfd;
    char buf[BUFSIZ];
    char cli_IP_str[INET_ADDRSTRLEN]; // IP地址占字节大小  //#define INET_ADDRSTRLEN 16
    int i, n;
    pid_t pid;

    // 回收子进程信号
    struct sigaction newact;
    newact.sa_handler = do_sigchild;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    sigaction(SIGCHLD, &newact, NULL);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0); // 1 socket

    int opt = 1; // SO_REUSEADDR为1,允许创建端口号相同但IP地址不同的多个socket描述符
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // 设置TCP属性 SO_REUSEADDR 端口复用

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // IPV4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //  INADDR_ANY 监听本地所有IP地址
    servaddr.sin_port = htons(SERV_PORT); // 端口

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)); // 2 bind

    Listen(listenfd, 128); // 3 listen

    printf("Accepting connections ...\n");

    while (1) {
        cliaddr_len = sizeof(cliaddr);
        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len); // 4 accept
        printf("client IP:%s, port:%d\n", inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, cli_IP_str, sizeof(cli_IP_str)),
               ntohs(cliaddr.sin_port));

        // 每次建立新连接，新建一个进程处理
        pid = fork();
        if (pid == 0) { // 子进程
            Close(listenfd); // 关闭监听
            while (1) {
                /*读取客户端发送数据*/
                n = Read(connfd, buf, sizeof(buf));
                if (n == 0) { // client closed
                    printf("the other side has been closed.\n");
                    break;
                } else if (n == -1) { // 出错
                    perror("read error");
                    exit(1);
                } else { // 正常读到的实际字节数
                    printf("received from %s at PORT %d\n",
                           inet_ntop(AF_INET, &cliaddr.sin_addr, cli_IP_str, sizeof(cli_IP_str)),
                           ntohs(cliaddr.sin_port));
                    Write(STDOUT_FILENO, buf, n); // cout

                    /*处理客户端数据*/
                    for (i = 0; i < n; i++) {
                        buf[i] = toupper(buf[i]);
                    }

                    /*处理完数据回写给客户端*/
                    Write(connfd, buf, n);
                }
            }
            Close(connfd); // 关闭连接
            return 0;
        } else if (pid > 0) { // 父进程
            Close(connfd); // 关闭连接
            // signal(SIGCHLD, wait_child); // signal 不需要 注册 sigaction
        } else { // error
            perr_exit("fork");
        }
    }
    return 0;
}
