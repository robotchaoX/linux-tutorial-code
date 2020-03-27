#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// ./a.out 8989 /home/chao/Desktop/
// 浏览器 http://192.168.1.106:8989/mk.txt

// 进程回收函数
void recyle(int num) {
  pid_t pid;
  while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
    printf("child died , pid = %d\n", pid);
  }
}

// 获取http请求行参数
void get_request_line_args(char *msg, char *method, char *path,
                           char *protocol) {
  // http请求行: // GET /xxx HTTP/1.1
  int i = 0;
  char *p = msg;
  while (*p != ' ') { // ' ' 分隔
    method[i++] = *p++;
  }
  p++;
  method[i] = 0;

  i = 0;
  while (*p != ' ') { // ' ' 分隔
    path[i++] = *p++;
  }
  p++;
  path[i] = 0;

  i = 0;
  while (*p != '\r' && *(p + 1) != '\n') { // '\r\n'结尾
    protocol[i++] = *p;
    p++;
  }
  protocol[i] = 0;
}

// 发送http响应头
void send_respond_head(int cfd, int size) {
  char buf[1024] = {0};
  // 状态行
  sprintf(buf, "http/1.1 200 OK\r\n");
  write(cfd, buf, strlen(buf));

  // 消息报头
  sprintf(buf, "Content-Type: text/plain\r\n");
  write(cfd, buf, strlen(buf));
  sprintf(buf, "Content-Length: %d\r\n", size);
  write(cfd, buf, strlen(buf));

  // 空行
  write(cfd, "\r\n", 2);
}

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    printf("eg: ./a.out port root_path\n");
    exit(1);
  }

  // 切换当前进程到http资源根目录
  chdir(argv[2]);
  printf("pwd:%s\n", argv[2]);
  printf("port:%s\n", argv[1]);
  printf("exe:%s\n", argv[0]);

  struct sockaddr_in serv_addr;
  socklen_t serv_len = sizeof(serv_addr);
  int port = atoi(argv[1]);

  // 创建套接字
  int lfd = socket(AF_INET, SOCK_STREAM, 0);
  // 初始化服务器 sockaddr_in
  memset(&serv_addr, 0, serv_len);
  serv_addr.sin_family = AF_INET;                // 地址族
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 监听本机所有的IP
  serv_addr.sin_port = htons(port);              // 设置端口
  // 绑定IP和端口
  int fl = 1;
  setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &fl, sizeof(fl));
  bind(lfd, (struct sockaddr *)&serv_addr, serv_len);

  // 设置同时监听的最大个数
  listen(lfd, 36);
  printf("Start accept ......\n");

  // 使用信号回收子进程pcb
  struct sigaction act;
  act.sa_handler = recyle;
  act.sa_flags = 0;
  sigemptyset(&act.sa_mask);
  sigaction(SIGCHLD, &act, NULL); // 捕获子进程结束信号

  struct sockaddr_in client_addr;
  socklen_t cli_len = sizeof(client_addr);
  while (1) {
    // 父进程接收连接请求
    // accept阻塞的时候被信号中断, 处理信号对应的操作之后
    // 回来之后不阻塞了, 直接返回-1, 这时候 errno==EINTR
    int cfd = accept(lfd, (struct sockaddr *)&client_addr, &cli_len);
    while (cfd == -1 && errno == EINTR) {
      cfd = accept(lfd, (struct sockaddr *)&client_addr, &cli_len);
    }
    printf("connect sucessful\n");
    // 创建子进程
    pid_t pid = fork();
    if (pid == 0) {
      close(lfd);
      // child process
      // 通信
      char ip[64];
      // client ip port
      printf("client IP: %s, port: %d\n",
             inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip, sizeof(ip)),
             ntohs(client_addr.sin_port));
      char buf[1024];
      int len = read(cfd, buf, sizeof(buf));
      if (len == -1) {
        perror("read error");
        exit(1);
      } else if (len == 0) {
        printf("客户端断开了连接\n");
        close(cfd);
        break;
      } else { // 建立了连接
        char method[12], path[1024], protocol[12];
        // 获取http请求参数
        get_request_line_args(buf, method, path, protocol);
        printf("method = %s, path = %s, protocol = %s\n", method, path,
               protocol);

        // 得到文件名 // 相对资源根目录/
        // char *file = path;
        char *file = path + 1; // 去除 资源根目录/
        printf("------ file = %s\n", file);
        // 获取文件属性
        struct stat st;
        int ret = stat(file, &st);
        if (ret == -1) {
          perror("stat error");
          exit(1);
        }

        // 发送http响应头
        send_respond_head(cfd, (int)st.st_size);
        // 发送http正文
        // 打开文件
        int fdd = open(file, O_RDONLY);
        // 读文件
        while ((len = read(fdd, buf, sizeof(buf))) > 0) {
          //发送数据给浏览器
          write(cfd, buf, len);
          write(STDOUT_FILENO, buf, len); // 打印输出
        }
      }
      // 干掉子进程
      return 0;

    } else if (pid > 0) {
      // parent process
      close(cfd);
    }
  }

  close(lfd);
  return 0;
}
