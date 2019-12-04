#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("./chown filename\n");
        exit(1);
    }

    // 修改所有者
    // user->ftp  group->ftp
    int ret = chown(argv[1], 116, 125);
    if (ret == -1) {
        perror("chown");
        exit(1);
    }
    return 0;
}
