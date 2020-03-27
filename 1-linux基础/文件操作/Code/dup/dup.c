#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int fd = open("a.txt", O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    printf("file open fd = %d\n", fd);

    // 找到进程文件描述表中 ==第一个== 可用的文件描述符
    // 将参数指定的文件复制到该描述符后，返回这个描述符
    // dup 复制参数fd所指的文件描述符,返回最小的尚未被使用过的文件描述符
    int ret = dup(fd); // 二个文件描述符指向同一文件
    if (ret == -1) {
        perror("dup");
        exit(1);
    }
    printf("dup fd = %d\n", ret);
    char *buf = "11111111111111111111111111\n";
    char *buf1 = "222222222222222222222222222\n";
    // 向原文件描述符写内容
    write(fd, buf, strlen(buf));
    // 向dup复制的文件描述符写内容，二者指向同一文件
    write(ret, buf1, strlen(buf1));

    close(fd);
    return 0;
}
