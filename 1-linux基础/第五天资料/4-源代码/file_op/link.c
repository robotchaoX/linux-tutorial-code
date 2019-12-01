#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("./link oldpath newpath\n");
        exit(0);
    }

    // 创建硬链接
    int ret = link(argv[1], argv[2]);
    if (ret == -1) {
        perror("link");
        exit(1);
    }
    return 0;
}
