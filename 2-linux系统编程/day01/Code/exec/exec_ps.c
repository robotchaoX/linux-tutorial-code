#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    int fd;

    fd = open("ps.out", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open ps.out error");
        exit(1);
    }
    dup2(fd, STDOUT_FILENO);

    execlp("ps", "ps", "ax", NULL); // exec只有失败了才返回
    perror("exec");
    exit(1);
    // close(fd); // 依赖进程结束系统回收

    return 0;
}
