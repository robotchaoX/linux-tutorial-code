#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("./mkdir newDir 764\n");
        exit(1);
    }

    int mode = strtol(argv[2], NULL, 8); // strtol字符串转长整型，8进制
    // 创建文件夹，指定权限
    int ret = mkdir(argv[1], mode);
    if (ret == -1) {
        perror("mkdir");
        exit(1);
    }
    return 0;
}
