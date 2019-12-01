#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

// 截断或扩展文件大小
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("./truncate filename filesize   \n");
        exit(1);
    }
    // 获取第二个参数值，大小, 字节
    long int len = strtol(argv[2], NULL, 10); // strtol字符串转长整型,10进制
    // 截取文件为指定大小
    int aa = truncate(argv[1], len);
    if (aa == -1) {
        perror("truncate");
        exit(1);
    }
    return 0;
}
