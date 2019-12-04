#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    // 打开文件
    int fd_new = open("english.txt", O_RDWR); // fd_new 被占用
    if (fd_new == -1) {
        perror("open");
        exit(1);
    }

    // 打开文件
    int fd_old = open("a.txt", O_RDWR);
    if (fd_old == -1) {
        perror("open");
        exit(1);
    }

    printf("fd_new = %d\n", fd_new);
    printf("fd_old = %d\n", fd_old);

    // dup2 复制文件描述符为指定值
    // #include <unistd.h>
    // int dup2(int fd_new, int fd_old);
    // dup2与dup区别是dup2可以用参数newfd指定新文件描述符的数值。若参数newfd已经被程序使用，则系统就会将newfd所指的文件关闭，若newfd等于oldfd，则返回newfd,而不关闭newfd所指的文件。
    int ret = dup2(fd_old, fd_new); // fd_new 被占用，则先将其关闭
    if (ret == -1) {
        perror("dup2");
        exit(1);
    }

    printf("current fd ret = %d\n", ret);
    printf("current fd fd_new = %d\n", fd_new);
    printf("current fd fd_old = %d\n", fd_old);

    char *buf = "主要看气质 ^_^！！！！！！！！！！\n";
    // fd_new fd_old 都指向同一文件fd_old
    write(fd_new, buf, strlen(buf));
    write(fd_old, "hello, world!", 13);

    close(fd_new);
    close(fd_old);
    return 0;
}
