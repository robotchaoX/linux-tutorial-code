#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// ./access english.txt
int main(int argc, char *argv[]) {
    if (argc < 2) {
        // argv[0] 执行的命令
        // argv[1] 命令的第一个参数
        printf("argc:%d ,    argv:%s\n", argc, argv[0]);
        printf("./access filename\n");
        exit(1);
    }

    // 获取文件权限
    int ret = access(argv[1], W_OK); // 是否可写
    if (ret == -1) {
        perror("access");
        exit(1);
    }
    printf("you can write this file : \"%s\" .\n", argv[1]);
    return 0;
}
