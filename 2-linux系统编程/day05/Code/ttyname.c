#include <stdio.h>
#include <unistd.h>

// 各种不同的终端所对应的设备文件名
int main(void) {
    printf("fd 0: %s\n", ttyname(0));
    printf("fd 1: %s\n", ttyname(1));
    printf("fd 2: %s\n", ttyname(2));
    // fd 0 : /dev/pts/1  不同终端值不同
    return 0;
}
