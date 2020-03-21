#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "wrap.h"

// #define MAXLINE 8192
#define SERV_PORT 8000

struct s_info { //定义一个结构体, 将地址结构跟cfd捆绑
    struct sockaddr_in cliaddr;
    int connfd;
};

// 线程回调函数
void *do_work(void *arg) {
    int n, i;
    struct s_info *ts = (struct s_info *)arg;
    char buf[BUFSIZ];
    char cli_IP_str[INET_ADDRSTRLEN]; //#define INET_ADDRSTRLEN 16  可用"[+d"查看

    while (1) {
        n = Read(ts->connfd, buf, sizeof(buf)); //读客户端
        if (n == 0) { // client closed
            printf("the client %d closed...\n", ts->connfd);
            break; //跳出循环,关闭cfd
        } else if (n == -1) {
            perror("read error");
            exit(1);
        } else {
            printf("received from %s at PORT %d\n",
                   inet_ntop(AF_INET, &(*ts).cliaddr.sin_addr, cli_IP_str, sizeof(cli_IP_str)),
                   ntohs((*ts).cliaddr.sin_port)); //打印客户端信息(IP/PORT)
            Write(STDOUT_FILENO, buf, n); //写出至屏幕

            /*处理客户端数据*/
            for (i = 0; i < n; i++) {
                buf[i] = toupper(buf[i]); //小写-->大写
            }

            Write(ts->connfd, buf, n); //回写给客户端
        }
    }
    Close(ts->connfd); // 关闭连接

    return (void *)0;
}

// 多线程并发服务器
// 每次建立新连接，新建一个线程处理
// 从客户端读字符，然后将每个字符转换为大写并回送给客户端
int main(void) {
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    int listenfd, connfd;
    pthread_t tid;
    struct s_info ts[256]; //根据最大线程数创建结构体数组.
    int i = 0;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0); // 1 创建一个socket, 得到lfd

    int opt = 1; // SO_REUSEADDR为1,允许创建端口号相同但IP地址不同的多个socket描述符
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // 设置TCP属性 SO_REUSEADDR 端口复用

    bzero(&servaddr, sizeof(servaddr)); //地址结构清零
    servaddr.sin_family = AF_INET; // ipv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //指定本地任意IP
    servaddr.sin_port = htons(SERV_PORT); //指定端口号

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)); // 2 绑定

    Listen(listenfd, 128); // 3 设置同一时刻链接服务器上限数

    printf("Accepting client connect ...\n");

    while (1) {
        cliaddr_len = sizeof(cliaddr);
        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len); // 4 accept 阻塞监听客户端链接请求
        char cli_IP[INET_ADDRSTRLEN]; //#define INET_ADDRSTRLEN 16
        printf("client IP:%s, port:%d\n", inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, cli_IP, sizeof(cli_IP)),
               ntohs(cliaddr.sin_port));

        ts[i].cliaddr = cliaddr;
        ts[i].connfd = connfd;

        // 每次建立新连接，新建一个线程处理
        /* 达到线程最大数时，pthread_create出错处理, 增加服务器稳定性 */
        pthread_create(&tid, NULL, do_work, (void *)&ts[i]);
        pthread_detach(tid); //子线程分离,防止僵线程产生.
        i++;
    }

    return 0;
}
