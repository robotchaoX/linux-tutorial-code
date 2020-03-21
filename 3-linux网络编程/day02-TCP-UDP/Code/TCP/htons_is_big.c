#include <arpa/inet.h>
#include <stdio.h>

int main(void) {
    short tmp = 0x1234;

    tmp = htons(tmp); // 主机字节序 转 网络字节序

    if ((*(char *)&tmp) == 0x34) {
        printf("It's little\n");
    } else if ((*(char *)&tmp) == 0x12) {
        printf("It's big\n");
    }

    return 0;
}
