#include "epoll_server.h"
#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// gcc main.c epoll_server.c
// a.out 8989 /home/chao/Desktop/
// 192.168.1.106:8989

int main(int argc, const char *argv[]) {
    if (argc != 3) {
        printf("eg: ./a.out port path\n");
        exit(1);
    }

    // 端口
    int port = atoi(argv[1]); // string to int
    if (port < 100) {
        printf("port 号太小，请再次运行输入!\n");
        exit(1);
    }
    printf("SERVER port: %s\n", argv[1]);
    // 修改进程的工作目录, 切换当前进程到http资源根目录
    int ret = chdir(argv[2]); //进入资源目录
    printf("SOURCE dir: %s\n", argv[2]);
    if (ret == -1) {
        perror("chdir error");
        exit(1);
    }

    // 启动epoll模型
    epoll_run(port);

    return 0;
}

#define MAXSIZE 2000 // epoll 挂载的最多节点数
void epoll_run(int port) {
    // 创建一个epoll树的根节点
    int epfd = epoll_create(MAXSIZE);
    if (epfd == -1) {
        perror("epoll_create error");
        exit(1);
    }

    // 添加要监听的节点 lfd
    int lfd = init_listen_fd(port, epfd);

    // 委托内核检测添加到树上的节点
    struct epoll_event allEvn[MAXSIZE];
    while (1) {
        int ret = epoll_wait(epfd, allEvn, MAXSIZE, -1); // 阻塞检测节点
        if (ret == -1) {
            perror("epoll_wait error");
            exit(1);
        }

        // 遍历发生变化的节点
        for (int i = 0; i < ret; ++i) {
            // 只处理读事件, 其他事件默认不处理
            struct epoll_event *pev = &allEvn[i];
            if (!(pev->events & EPOLLIN)) {
                // 不是读事件
                printf("只处理读事件EPOLLIN\n");
                continue;
            }
            if (pev->data.fd == lfd) { // 监听节点 lfd
                // 接受连接请求
                do_accept(lfd, epfd);
            } else { // 建立连接添加的普通节点
                // todo 多线程读取数据
                // 读数据
                do_read(pev->data.fd, epfd);
            }
        }
    }
}

// 初始化监听套接字
int init_listen_fd(int port, int epfd) {
    //　创建监听的套接字 lfd
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket error");
        exit(1);
    }

    // lfd绑定本地IP和port
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port); // "Host to Network Short"本地字节序to网络字节序
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    // 端口复用
    int flag = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    // 绑定
    int ret = bind(lfd, (struct sockaddr *)&serv, sizeof(serv));
    if (ret == -1) {
        perror("bind error");
        exit(1);
    }

    // 设置监听
    ret = listen(lfd, 64);
    if (ret == -1) {
        perror("listen error");
        exit(1);
    }

    // lfd添加到epoll树上
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = lfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    if (ret == -1) {
        perror("epoll_ctl add lfd error");
        exit(1);
    }

    return lfd;
}

// 接受新连接处理
void do_accept(int lfd, int epfd) {
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    // 建立新连接 cfd
    int cfd = accept(lfd, (struct sockaddr *)&client, &len);
    if (cfd == -1) {
        perror("accept error");
        exit(1);
    }

    // 打印客户端信息
    char ip[64] = {0};
    printf("connect sucessful\n");
    printf("New Client IP: %s, Port: %d, cfd = %d\n", inet_ntop(AF_INET, &client.sin_addr.s_addr, ip, sizeof(ip)),
           ntohs(client.sin_port), cfd);

    // 设置cfd为非阻塞
    int flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);

    // 新建立的连接cfd节点挂到epoll树上
    struct epoll_event ev;
    ev.data.fd = cfd;
    // 边沿非阻塞模式
    ev.events = EPOLLIN | EPOLLET;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    if (ret == -1) {
        perror("epoll_ctl add cfd error");
        exit(1);
    }
}

// 读http请求数据
void do_read(int cfd, int epfd) {
    // 将浏览器发过来的数据, 读到buf中
    char line[1024] = {0};
    // 1.读http请求行(一行)
    // -----: GET /x-dir-x HTTP/1.1
    int len = get_line(cfd, line, sizeof(line));
    if (len == 0) { // 发来的所有请求数据读完了
        printf("客户端断开连接...\n");
        // 关闭套接字, cfd从epoll上del
        disconnect(cfd, epfd);
    } else if (len == -1) {
        printf("get_line error\n");
        // 关闭套接字, cfd从epoll上del
        disconnect(cfd, epfd);
        exit(1);
    } else {
        printf("============= http 请求信息 ============\n");
        printf("======= 请求行 \n-----: %s", line);
        printf("======= 请求头 \n");
        // 还有数据没读完,继续读
        // 2. 读http请求头(多行)
        /*
        -----: Host: 192.168.1.106:8989
        -----: Connection: keep-alive
        -----: Upgrade-Insecure-Requests: 1
        -----: DNT: 1
        -----: User-Agent: Chrome/78.0.3904.97 Safari/537.36
        -----: Accept:text/html,application/xhtml+xml,image/webp,image
        -----: Referer : http : // 192.168.1.106:8989/
        -----: Accept - Encoding : gzip,deflate
        -----: Accept - Language : zh - CN, zh; q = 0.9, en;
        */
        while (len) {
            char buf[1024] = {0};
            len = get_line(cfd, buf, sizeof(buf));
            printf("-----: %s", buf);
        }
        printf("============= http 请求信息 End ============\n");
    }

    // 请求行: // GET /xxx HTTP/1.1
    // 关键数据: 请求的路径名 /xxx
    // 判断是不是get请求
    if (strncasecmp("get", line, 3) == 0) { // get请求
        // 处理http get请求
        http_request(line, cfd);
        // 关闭套接字, cfd从epoll上del
        disconnect(cfd, epfd);
    }
}

// 解析http请求消息的每一行内容
int get_line(int sock, char *buf, int size) {
    int i = 0;
    char c = '\0';
    int n;
    while ((i < size - 1) && (c != '\n')) {
        n = recv(sock, &c, 1, 0); // 一个一个字节读取
        if (n > 0) {
            if (c == '\r') { // http请求消息每行以 \r\n结尾
                n = recv(sock, &c, 1, MSG_PEEK);
                if ((n > 0) && (c == '\n')) {
                    recv(sock, &c, 1, 0);
                } else {
                    c = '\n';
                }
            }
            buf[i] = c;
            i++;
        } else {
            c = '\n';
        }
    }
    buf[i] = '\0';

    //   if (n == -1) { // 出错
    //     i = -1;
    //   }

    return i; // 读取一行的字节数
}

// 断开连接的函数
void disconnect(int cfd, int epfd) {
    // 从epoll树epfd上删除cfd节点
    int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
    if (ret == -1) {
        perror("epoll_ctl del cfd error");
        exit(1);
    }
    close(cfd);
}

// 处理http get请求
void http_request(const char *request, int cfd) {
    // 拆分http请求行
    // get /xxx http/1.1
    char method[12], path[1024], protocol[12];
    // 正则表达式匹配,拆分字符串,以空格分隔
    sscanf(request, "%[^ ] %[^ ] %[^ ]", method, path, protocol);
    printf("method = %s, path = %s, protocol = %s\n", method, path, protocol);

    char deco_path[1024];
    char oripath[1024];
    strcpy(oripath, path);
    // 解码 http %23 %34 %5f -> 中文 unicode编码
    // 解码 /%e4%b8%ad%e6%96%87/ -> /中文/ (e4b8ade69687)
    // 中 UTF-8: e4 b8 ad UTF-16BE: 4e2d Decimal: &#20013; Octal: \047055
    // 文 UTF-8: e6 96 87 UTF-16BE: 6587 Decimal: &#25991; Octal: \062607
    decode_str(path, path); // 解析http默认处理的中文
    printf("++decode_str originpath: %s --> decopath: %s\n", oripath, path);

    // 处理path  /xxx
    // 去掉path中前面的/,只留路径名字
    char *file = path + 1;
    // 如果没有指定访问的资源, 默认显示资源目录中的内容
    if (strcmp(path, "/") == 0) { // http空请求时 GET / HTTP/1.1
        // file的值, 资源目录的当前位置
        file = "./";
    }

    // 获取文件属性
    struct stat st;
    int ret = stat(file, &st);
    if (ret == -1) {
        // show 404
        send_respond_head(cfd, 404, "File Not Found", ".html", -1);
        send_file(cfd, "404.html");
    }

    // 判断是目录还是文件
    // 如果是目录
    if (S_ISDIR(st.st_mode)) {
        // 发送头信息
        send_respond_head(cfd, 200, "OK", get_file_type(".html"), -1);
        // 发送目录信息
        send_dir(cfd, file);
    } else if (S_ISREG(st.st_mode)) {
        // 文件
        // 发送消息报头
        send_respond_head(cfd, 200, "OK", get_file_type(file), st.st_size);
        // 发送文件内容
        send_file(cfd, file);
    }
}

// 发送http响应头
void send_respond_head(int cfd, int no, const char *desp, const char *type, long len) {
    char buf[1024] = {0};
    // 响应状态行
    sprintf(buf, "http/1.1 %d %s\r\n", no, desp); // http/1.1 200 OK
    send(cfd, buf, strlen(buf), 0);
    printf("============= 响应状态行 ============\n-----: %s", buf);
    // 响应消息报头
    sprintf(buf, "Content-Type:%s\r\n; charset=UTF-8", type);
    send(cfd, buf, strlen(buf), 0); // 当send的flags为0时,等价 write(cfd, buf, strlen(buf));
    sprintf(buf, "Content-Length:%ld\r\n", len);
    send(cfd, buf, strlen(buf), 0);
    printf("============= 响应消息报头 ============\n-----: %s", buf);
    // 空行
    send(cfd, "\r\n", 2, 0);
    printf("-----: %s", "\r\n");
    printf("============= 响应消息 End ============\n");
}

// 发送目录内容
void send_dir(int cfd, const char *dirname) {
    // 拼一个html页面 <html>  <table> </table>  </html>
    char buf[4094] = {0};

    sprintf(buf, "<html><head><title>目录名: %s</title></head>", dirname);
    sprintf(buf + strlen(buf), "<body><h1>当前目录: %s</h1><table>", dirname);

    // 拼接目录表格 <table> </table>的内容
    char enstr[1024] = {0};
    char path[1024] = {0};
    // 目录项二级指针
    struct dirent **ptr;
    // scandir
    int num = scandir(dirname, &ptr, NULL, alphasort);
    // 遍历 显示所有目录项
    for (int i = 0; i < num; ++i) {
        char *name = ptr[i]->d_name;
        // 拼接文件的完整路径
        sprintf(path, "%s%s", dirname, name); // dirname目录结尾带/
        printf("path = %s \n", path);
        struct stat st;
        stat(path, &st);

        // encode_str模仿http 默认对中文的处理方式(可省)
        // 编码 将中文unicode编码转换为html中对应编码
        // 编码 中文(0xe4 0xb8 0xad) -> %e4 %b8 %ad
        // 编码 中文(e4b8ade69687) -> %e4%b8%ad%e6%96%87
        encode_str(enstr, sizeof(enstr), name); // 模仿http默认对中文的处理方式(可省)
        printf("++encode_str name: %s --> enstr: %s\n", name, enstr);

        // 如果是文件
        if (S_ISREG(st.st_mode)) {
            sprintf(buf + strlen(buf), "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>", enstr, name,
                    (long)st.st_size); // 超链接
        }
        // 如果是目录
        else if (S_ISDIR(st.st_mode)) {
            sprintf(buf + strlen(buf), "<tr><td><a href=\"%s/\">%s/</a></td><td>%ld</td></tr>", enstr, name,
                    (long)st.st_size);
        }
        send(cfd, buf, strlen(buf), 0);
        memset(buf, 0, sizeof(buf));
        // 字符串拼接
    }

    sprintf(buf + strlen(buf), "</table></body></html>");
    send(cfd, buf, strlen(buf), 0);

    printf("dir message send OK!!!!\n");
#if 0
    // 打开目录
    DIR* dir = opendir(dirname);
    if(dir == NULL)
    {
        perror("opendir error");
        exit(1);
    }

    // 读目录
    struct dirent* ptr = NULL;
    while( (ptr = readdir(dir)) != NULL )
    {
        char* name = ptr->d_name;
    }
    closedir(dir);
#endif
}

// 发送文件
void send_file(int cfd, const char *filename) {
    // 打开文件
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        // show 404
        return;
    }

    // 循环读文件
    char buf[4096] = {0};
    int len = 0;
    while ((len = read(fd, buf, sizeof(buf))) > 0) {
        // 发送读出的数据
        send(cfd, buf, len, 0);
    }
    if (len == -1) {
        perror("read file error");
        exit(1);
    }
    printf("file message send OK!!!!\n");
    close(fd);
}

// 16进制数转化为10进制
int hexit(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    return 0;
}

/*
 *  这里的内容是处理%20之类的东西！是"解码"过程。
 *  %20 URL编码中的‘ ’(space)
 *  %21 '!' %22 '"' %23 '#' %24 '$'
 *  %25 '%' %26 '&' %27 ''' %28 '('......
 *  相关知识html中的‘ ’(space)是&nbsp
 */
// encode_str模仿http 默认对中文的处理方式(可省)
// 编码 将中文unicode编码转换为html中对应编码
// 编码 中文(0xe4 0xb8 0xad) -> %e4 %b8 %ad
// 编码 中文(e4b8ade69687) -> %e4%b8%ad%e6%96%87
void encode_str(char *to, int tosize, const char *from) {
    int tolen;
    for (tolen = 0; *from != '\0' && tolen + 4 < tosize; ++from) {
        if (isalnum(*from) || strchr("/_.-~", *from) != (char *)0) { // ascii不变
            *to = *from;
            ++to;
            ++tolen;
        } else { // 中文(0xe4 0xb8 0xad) -> %e4 %b8 %ad 格式
            sprintf(to, "%%%02x", (int)*from & 0xff);
            to += 3;
            tolen += 3;
        }
    }
    *to = '\0';
}

// 解码 http格式 %e4 %b8 %ad -> 中文(0xe4 0xb8 0xad) unicode编码
// 解码 /%e4%b8%ad%e6%96%87/ -> /中文/ (e4b8ade69687)
void decode_str(char *to, char *from) {
    for (; *from != '\0'; ++to, ++from) {
        if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2])) { // %e4 %b8 %ad 格式 -> 中文(0xe4 0xb8 0xad)
            *to = hexit(from[1]) * 16 + hexit(from[2]);
            from += 2;
        } else {
            *to = *from;
        }
    }
    *to = '\0';
}

// 通过文件名获取文件的类型
const char *get_file_type(const char *name) {
    char *dot;
    // 自右向左查找‘.’字符, 如不存在返回NULL
    dot = strrchr(name, '.'); //  Find the last occurrence of C in S.
    if (dot == NULL)
        return "text/plain; charset=utf-8";
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
        return "text/html; charset=utf-8";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(dot, ".gif") == 0)
        return "image/gif";
    if (strcmp(dot, ".png") == 0)
        return "image/png";
    if (strcmp(dot, ".css") == 0)
        return "text/css";
    if (strcmp(dot, ".au") == 0)
        return "audio/basic";
    if (strcmp(dot, ".wav") == 0)
        return "audio/wav";
    if (strcmp(dot, ".avi") == 0)
        return "video/x-msvideo";
    if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
        return "video/quicktime";
    if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
        return "video/mpeg";
    if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
        return "model/vrml";
    if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
        return "audio/midi";
    if (strcmp(dot, ".mp3") == 0)
        return "audio/mpeg";
    if (strcmp(dot, ".ogg") == 0)
        return "application/ogg";
    if (strcmp(dot, ".pac") == 0)
        return "application/x-ns-proxy-autoconfig";

    return "text/plain; charset=utf-8";
}
