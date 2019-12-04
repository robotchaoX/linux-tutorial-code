#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("./opendir path\n");
        exit(1);
    }

    // 打开目录
    DIR *dir = opendir(argv[1]);
    if (dir == NULL) {
        perror("opendir");
        exit(1);
    }

    // char buf[512];
    // getcwd(buf, sizeof(buf)); // 当前路径
    // printf("current dir: %s\n", buf);

    // 关闭目录
    closedir(dir);

    return 0;
}
