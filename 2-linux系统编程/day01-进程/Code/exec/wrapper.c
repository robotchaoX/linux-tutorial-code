#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // if (argc != 2) {
    //     fputs("usage: wrapper file\n", stderr);
    //     exit(1);
    // }
    argv[1] = "upper.txt";

    int fd;
    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    // 从文件输入而不是键盘输入
    dup2(fd, STDIN_FILENO); // 断开STDIN_FILENO与键盘, STDIN_FILENO和fd都与文件绑定
    close(fd);

    execl("./_upper", "_upper", NULL);
    perror("exec ./upper");
    exit(1);

    return 0;
}
