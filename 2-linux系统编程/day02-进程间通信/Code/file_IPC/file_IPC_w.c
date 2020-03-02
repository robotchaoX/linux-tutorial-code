/*
 * 先执行,将数据写入文件test.txt
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define N 6

int main(void) {
    char buf[1024];
    char *str = "--------------1111 secesuss-------------\n";
    int ret;

    int fd = open("file_IPC.txt", O_RDWR | O_TRUNC | O_CREAT, 0664);

    //直接打开文件写入数据
    write(fd, str, strlen(str));
    printf("test1 write into test.txt finish\n");

    sleep(N);

    lseek(fd, 0, SEEK_SET);
    ret = read(fd, buf, sizeof(buf));
    ret = write(STDOUT_FILENO, buf, ret);

    if (ret == -1) {
        perror("write second error");
        exit(1);
    }

    close(fd);

    return 0;
}
