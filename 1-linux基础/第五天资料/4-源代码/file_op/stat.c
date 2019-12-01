#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("./stat filename\n");
        exit(1);
    }

    struct stat buf_st;
    // int ret = stat(argv[1], &buf_st); // 穿透软链接，获取软链接指向的文件的状态
    int ret = lstat(argv[1], &buf_st); // 不穿透软链接，获取软链接的状态
    if (ret == -1) {
        perror("stat");
        exit(1);
    }

    printf("file size = %d\n", (int)buf_st.st_size);

    return 0;
}
