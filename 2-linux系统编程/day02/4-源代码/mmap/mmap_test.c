#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
    int fd = open("./test.txt", O_RDWR);
    char *p;
    int i;

    struct stat sbuf;
    stat("./test.txt", &sbuf); // 获取文件大小
    int len = sbuf.st_size;
    // int len = ftruncate(fd, 7); // ftruncate 设置文件大小
    // if (len == -1) {
    //     perror("ftruncate error:");
    //     exit(1);
    // }
    printf("len = %d\n", len);

    // 创建映射区 mmap
    p = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    // p = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }

    strcpy(p, "hehehe"); // 写数据
    for (i = 0; i < len; i++) {
        printf("%c", p[i]);
    }
    printf("\n");

    munmap(p, len); // munmap 释放映射区mmap
    close(fd);

    return 0;
}
