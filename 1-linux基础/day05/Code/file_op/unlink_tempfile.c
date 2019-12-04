#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// 程序执行结束之后临时文件就被删除
int main(void) {
    // 打开临时文件
    int fd = open("tempfile", O_CREAT | O_RDWR, 0755);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    // 解除硬链接，即临时删除文件 // unlink: Remove files via the unlink syscall
    // 如果文件正在打开，则等到文件关闭后再删除
    int ret = unlink("tempfile");
    if (ret == -1) {
        perror("unlink");
        exit(1);
    }

    char buf[512];
    write(fd, "hello\n", 6); // 向tempfile写入内容
    // 重置文件指针
    lseek(fd, 0, SEEK_SET); // 文件指针移动到开头
    int len = read(fd, buf, sizeof(buf)); // 读取tempfile写入的内容
    // 将读到的内容写到屏幕输出 STDOUT_FILENO
    write(STDOUT_FILENO, buf, len); // STDOUT_FILENO ：1

    close(fd); // 关闭文件，删除tempfile

    return 0;
}
