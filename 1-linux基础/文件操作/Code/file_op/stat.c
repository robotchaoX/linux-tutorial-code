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

    struct stat st;
    int ret = stat(argv[1], &st); // 穿透软链接，获取软链接指向的文件的状态
    // int ret = lstat(argv[1], &st); // 不穿透软链接，获取软链接的状态
    if (ret == -1) {
        perror("stat");
        exit(1);
    }

    // 文件大小
    int fileSize = (int)st.st_size;
    // 硬链接计数
    int linkNum = st.st_nlink;
    // 修改时间
    char *time = ctime(&st.st_mtime);

    printf("file size = %d\n", fileSize);

    return 0;
}
