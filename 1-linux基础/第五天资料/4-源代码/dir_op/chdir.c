#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("./chdir dir\n");
        exit(1);
    }

    // 当前线程切换到目录
    int ret = chdir(argv[1]);
    if (ret == -1) {
        perror("chdir");
        exit(1);
    }

    // 在目录中创建文件
    int fd = open("chdir.txt", O_CREAT | O_RDWR, 0777);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    close(fd);

    char buf[128];
    // getcwd当前目录
    getcwd(buf, sizeof(buf));
    printf("current dir: %s\n", buf);

    return 0;
}
