#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
    int fd = open("mmapfile.txt", O_RDWR | O_CREAT | O_TRUNC, 0764);
    // int fd = open("./mmapfile.txt", O_RDWR | O_CREAT, 644);
    if (fd < 0)
        perror("open error");
    char *p;
    int i;

    // struct stat sbuf;
    // stat("./mmapfile.txt", &sbuf); // stat获取文件大小
    // int len = sbuf.st_size;
    int len = 8;
    int ret = ftruncate(fd, len); // 拓展文件大小,不能创建大小为0的mmap
    if (ret == -1) {
        perror("ftruncate error:");
        exit(1);
    }
    printf("len = %d\n", len);

    // 创建映射区 mmap
    p = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // 同步到文件
    // p = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }
    close(fd); // 创建mmap后可立即关闭文件

    // ! 超过了内存映射区大小会怎样？？写入文件的大小会截断
    strcpy(p, "1234567890123456789"); // 写数据
    for (i = 0; i < len; i++) {
        printf("%c", p[i]);
    }
    printf("\n");
    printf("%s\n", p);

    munmap(p, len); // munmap 释放映射区mmap

    return 0;
}
