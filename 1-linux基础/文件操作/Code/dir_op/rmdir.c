#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("./rmdir dir \n");
        exit(1);
    }

    // 删除目录,只能删除空目录??
    int ret = rmdir(argv[1]);
    if (ret == -1) {
        perror("rmdir");
        exit(1);
    }
    return 0;
}
