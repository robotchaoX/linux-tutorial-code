#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dir.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// gcc main.c epoll_server.c
// a.out 8989 /home/chao/Desktop/
// 192.168.1.106:8989

#define SERVER_PORT 8989
const char *workdir = "./";
#define FILE_404 "my404.html"

typedef struct sockinfo {
  int fd;
  struct sockaddr_in sock;
} SockInfo;

// 发送http协议响应头
void http_respond_head(int cfd, char *type, int size) {
  char buf[1024] = {0};
  // 状态行
  sprintf(buf, "http/1.1 200 OK\r\n"); // http/1.1 200 OK
  send(cfd, buf, strlen(buf), MSG_NOSIGNAL);
  // 消息报头
  sprintf(buf, "Content-Type: %s ; charset=UTF-8\r\n", type);
  send(cfd, buf, strlen(buf), MSG_NOSIGNAL);
  sprintf(buf, "Content-Length: %d\r\n", size);
  send(cfd, buf, strlen(buf), MSG_NOSIGNAL);
  // 空行
  send(cfd, "\r\n", 2, MSG_NOSIGNAL);
}

// 404not_found

void not_found(int cfd) {
  struct stat st;
  stat(FILE_404, &st);
  http_respond_head(cfd, "text/html", (int)st.st_size);

  int fd404 = open(FILE_404, O_RDONLY);
  if (-1 == fd404) {
    perror("open err");
    exit(1);
  }
  char buf[1024] = {0};
  int len;
  while ((len = read(fd404, buf, sizeof(buf))) > 0) {
    write(cfd, buf, len);
  }
  if (0 == len) {
    close(fd404);
  }
  if (-1 == len) {
    perror("read err");
    exit(1);
  }
}

/**文件IO相关操作***/

//判断文件/目录 是否存在
int isExist(const char *path) {
  int ret = access(path, F_OK);
  if (0 == ret) {
    return 1;
  }
  return 0;
}

//判断是否是目录
int isDir(const char *path) {
  struct stat st;
  stat(path, &st);
  return S_ISDIR(st.st_mode);
}

void showDir(const char *path) {
  char buf[1024] = {0};

  int tempfd = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
  if (-1 == tempfd) {
    perror("open temp.txt err");
    exit(1);
  }
  sprintf(buf, "<font color=\"blue\" size=\"7\">当前路径:%s<br/></font>\n",
          path);
  write(tempfd, buf, strlen(buf));

  DIR *pdir;
  struct dirent *pDirent;
  pdir = opendir(path);
  if (pdir) {
    while ((pDirent = readdir(pdir)) != NULL) {
      if (strcmp(pDirent->d_name, ".") == 0) {
        continue;
      }

      if (strcmp(pDirent->d_name, "..") == 0) {
        continue;
      }

      char linkname[256] = "http://192.168.18.133:8888/";

      strcat(linkname, path + strlen(workdir)); //跳过资源根路径
      // printf("linkname=%s\n", linkname);
      if (strcmp(path + strlen(workdir), "")) {
        strcat(linkname, "/"); // path不是资源根目录，才需要加/
      }
      strcat(linkname, pDirent->d_name);
      sprintf(buf, "<a href=\"%s\" target=\"_blank\">%s</a><br/>\n", linkname,
              pDirent->d_name);
      write(tempfd, buf, strlen(buf));
    }

  } else {
    printf("opendir err\n");
  }
  closedir(pdir);
  close(tempfd);
}

int main(int argc, char *argv[]) {
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
  int err = chdir(argv[2]); //进入资源目录
  printf("SOURCE dir: %s\n", argv[2]);
  if (err == -1) {
    perror("chdir error");
    exit(1);
  }

  //创建套接字
  int lfd = socket(AF_INET, SOCK_STREAM, 0);

  //端口复用
  int flag = 1;
  setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
  //绑定服务器IP 端口
  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  int ret = bind(lfd, (struct sockaddr *)&server, sizeof(server));
  if (-1 == ret) {
    perror("bind err");
    exit(1);
  }

  //设置同时监听的最大个数
  listen(lfd, 66);
  printf("start to accept ......\n");

  //创建红黑树理论根节点个数
  int epfd = epoll_create(2000);
  if (-1 == epfd) {
    perror("epoll_create err");
    exit(1);
  }
  //将lfd添加到监听列表
  SockInfo *sinfo = (SockInfo *)malloc(sizeof(SockInfo));
  sinfo->sock = server; //结构体直接赋值
  sinfo->fd = lfd;

  struct epoll_event ev;
  ev.data.ptr = sinfo;
  ev.events = EPOLLIN;
  ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
  if (-1 == ret) {
    perror("epoll_ctl err");
    exit(1);
  }

  struct epoll_event all[2000]; //存epoll_wait传出参数
  //通信
  while (1) {
    //设置监听  -1一直阻塞，直到有事件才返回
    int num = epoll_wait(epfd, all, sizeof(all) / sizeof(all[0]), -1);
    if (-1 == num) {
      perror("epoll_wait err");
      exit(1);
    }

    //遍历前num个元素
    int i;
    for (i = 0; i < num; ++i) {
      //如果不是读事件，跳过本次循环
      if (all[i].events != EPOLLIN) {
        continue;
      }
      int fd = ((SockInfo *)all[i].data.ptr)->fd; //拿到fd

      //如果fd是监听lfd,说明有新的连接到来
      if (fd == lfd) {
        char ipbuf[64];
        SockInfo *info = (SockInfo *)malloc(sizeof(SockInfo));
        struct sockaddr_in client;
        socklen_t cli_len = sizeof(client);
        int cfd = accept(lfd, (struct sockaddr *)&client, &cli_len);
        if (-1 == cfd) {
          perror("accept err");
          exit(1);
        }
        info->fd = cfd;
        info->sock = client; //保存新连接的客户端的信息

        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.ptr = info; // void * 指向 info

        //挂到红黑树上
        ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
        if (-1 == ret) {
          perror("epoll_ctl err");
          exit(1);
        }
        printf(
            "client connected, fd = %d, IP = %s, Port = %d\n", cfd,
            inet_ntop(AF_INET, &client.sin_addr.s_addr, ipbuf, sizeof(ipbuf)),
            ntohs(client.sin_port));
      } else {
        //通信
        char ipbuf[64]; //用于通信的时候打印对应客户端的IP
        char buf[1024] = {0};
        SockInfo *p =
            (SockInfo *)all[i].data.ptr; //取出指向SockInfo结构体变量的指针
        //拿到ip,后面用于显示哪个客户端
        inet_ntop(AF_INET, &p->sock.sin_addr.s_addr, ipbuf, sizeof(ipbuf));

        int len = recv(fd, buf, sizeof(buf), 0);
        if (-1 == len) {
          perror("recv error");
          exit(1);
        } else if (0 == len) {
          inet_ntop(AF_INET, &p->sock.sin_addr.s_addr, ipbuf, sizeof(ipbuf));
          printf("client %d 已经断开连接, IP = %s, Port = %d\n", fd, ipbuf,
                 ntohs(p->sock.sin_port));
          //从树上摘下节点
          epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
          close(fd); //先摘下节点，才能关闭fd文件
          if (p != NULL) {
            free(p);
            p = NULL;
          }
        } else {

          printf("浏览器请求:%s\n", buf);
          //读数据
          //先把buf中的请求行拿出来
          char pathbuf[256] = {0}; //存浏览器请求的服务器路径
          char path[256] = {0};    //存服务器中的实际路径
          // path里面存着 /文件路径
          sscanf(buf, "%*[^ ] %[^ ]", pathbuf);
          //拼成服务器中的实际目录
          sprintf(path, "%s%s", workdir, pathbuf + 1);

          printf("path=%s\n", path);
          //如果路径不存在，报404NOTFOUND
          if (!isExist(path)) {
            not_found(fd);
            // close(fd);
            continue;
          }

          //如果请求的是 资源根目录 或 资源根目录下的文件夹
          if (isDir(path) || strcmp(pathbuf, "/") == 0) {
            //服务器根目录
            if (strcmp(path, "/") == 0) {
              showDir("./");
            } else {
              showDir(path);
            }
            struct stat st;
            stat("temp.txt", &st);
            int tempfd = open("temp.txt", O_RDONLY);
            if (-1 == tempfd) {
              printf("%s 打开错误!\n", path);
              perror("open err");
              exit(1);
            }
            http_respond_head(fd, "text/html", (int)st.st_size);
            while ((len = read(tempfd, buf, sizeof(buf))) > 0) {
              write(fd, buf, len);
            }

            close(tempfd);
          } else //不是目录
          {
            //找出来后缀
            char *type = path + strlen(path); // type指向path结尾
            while (type >= path) {
              if (*type == '.') {
                break;
              }
              --type;
            }
            // printf("type=%s\n", type);//打印后缀
            char httptype[20] = {0};
            if (strcmp(type, ".html") == 0)
              strcpy(httptype, "text/html");
            else if (strcmp(type, ".jpg") == 0)
              strcpy(httptype, "image/jpeg");
            else if (strcmp(type, ".png") == 0)
              strcpy(httptype, "image/png");
            else if (strcmp(type, ".mp3") == 0)
              strcpy(httptype, "audio/mpeg");
            else if (strcmp(type, ".avi") == 0)
              strcpy(httptype, "video/x-msvideo");
            else if (strcmp(type, ".mp4") == 0)
              strcpy(httptype, "video/mpeg");
            else
              strcpy(httptype, "text/plain"); //否则就当成普通文件

            int rfd;
            struct stat st;
            int flag = 0;
            /*	if(strcmp(type, ".mp4") == 1)
                    {
                            flag = 1;
                            rfd = open("mymp4.html", O_RDONLY);
                            strcpy(httptype, "text/html");
                    //	astruct stat st;
                            stat("mymp4.html", &st);

                    } */

            {
              rfd = open(path, O_RDONLY);

              //	struct stat st;
              stat(path, &st);
            }
            if (-1 == rfd) {
              perror("open err");
              exit(1);
            }

            printf("httptype=%s\n", httptype);
            http_respond_head(fd, httptype, (int)st.st_size);
            char bigbuf[8192] = {0};
            printf("开始传%s\n", path);
            while ((len = read(rfd, bigbuf, sizeof(bigbuf))) > 0) {
              //	write(fd, bigbuf, len);
              send(fd, bigbuf, len, MSG_NOSIGNAL);
            }
            close(rfd);

            /*	if(flag == 1)
                    {
                            stat(path, &st);
                            http_respond_head(fd, "text/plain",
               (int)st.st_size); int rfd = open(path, O_RDONLY); if(-1 == rfd)
                            {
                                    perror("open err");
                                    exit(1);
                            }

                            while((len = read(rfd, bigbuf, sizeof(bigbuf)))>0)
                            {
                    //	write(fd, bigbuf, len);
                            send(fd, bigbuf, len , MSG_NOSIGNAL);
                            }
                            close(rfd);
                    }

            */
          }
        }
      }
    }
  }

  printf("服务器断开连接!\n");

  close(lfd);
  if (sinfo != NULL) {
    free(sinfo);
    sinfo = NULL;
  }
  close(epfd);
  return 0;
}
