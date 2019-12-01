#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("./symlink oldpath newpath\n");
        exit(1);
    }

    // symlink 创建软链接
    int ret = symlink(argv[1], argv[2]);
    if (ret == -1) {
        perror("symlink");
        exit(1);
    }

    return 0;
}
