#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 结果写入 exec-ps.txt 文件
int main(void) {
    int fd;

    fd = open("exec-ps.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open ps.out error");
        exit(1);
    }
    dup2(fd, STDOUT_FILENO); // 关闭STDOUT_FILENO和显示器绑定,都与fd文件绑定
    // cout输出到了文件里面，而不是输出到了显示器
    close(fd);

    execlp("ps", "ps", "ax", NULL); // exec只有失败了才返回
    perror("exec");
    exit(1);

    return 0;
}
