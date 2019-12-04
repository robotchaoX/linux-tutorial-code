#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("./rename oldName newName\n");
        exit(1);
    }

    // rename 重命名
    int ret = rename(argv[1], argv[2]);
    if (ret == -1) {
        perror("rename");
        exit(1);
    }
    return 0;
}
