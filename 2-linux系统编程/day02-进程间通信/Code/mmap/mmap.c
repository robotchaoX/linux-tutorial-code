#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void sys_err(char *str) {
    perror(str);
    exit(1);
}

int main(void) {
    char *mem;
    int len = 0;

    // 打开新建文件
    int fd = open("mmapfile.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    // int fd = open("dict.txt", O_RDWR); // 打开文件
    if (fd < 0)
        sys_err("open error");
    // unlink("mmapfile.txt"); //删除临时文件目录项,使之具备被释放条件.
    len = 10;
    int ret = ftruncate(fd, len); // 拓展文件大小 //  success returned 0, error -1
    if (ret == -1) {
        perror("ftruncate:");
    }
    /*完成文件拓展
        len = lseek(fd, 3, SEEK_SET);   //获取文件大小,根据文件大小创建映射区
        write(fd, "e", 1);              //实质性完成文件拓展
        */
    printf("The length of file = %d\n", len);

    // 创建内存映射mmap
    mem = mmap(NULL, len, PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) { //出错判断
        sys_err("mmap err: ");
    }
    close(fd); // 创建mmap后可立即关闭文件

    // 读写内存映射区
    strcpy(mem, "xxx1234567891111111222222222333333333444444444555555555"); // 同步写入文件的大小限定在指定长度len20
    printf("sizeof(mem) %lu , %s\n", sizeof(mem), mem); // mem 只是指针，内存映射区首地址
    // ! 超过了内存映射区大小会怎样？？

    // 释放mmap
    if (munmap(mem, len) < 0)
        sys_err("munmap");

    return 0;
}

//思考：
// 0. 可以open的时候O_CREAT一个新文件来创建映射区吗?
// 1. 如果mem++，munmap可否成功？
// 2. 如果open时O_RDONLY, mmap时PROT参数指定PROT_READ|PROT_WRITE会怎样？
// 3. 如果文件偏移量为1000会怎样？
// 4. 如果不检测mmap的返回值，会怎样？
// 5. mmap什么情况下会调用失败？
// 6. 对mem越界操作会怎样？
// 7. 文件描述符先关闭，对mmap映射有没有影响？
