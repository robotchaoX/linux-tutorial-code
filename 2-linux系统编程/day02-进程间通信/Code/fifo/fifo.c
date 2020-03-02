#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFO_PATH "./my_fifo"

void sys_err(char *str) {
    perror(str);
    exit(-1);
}

// 回收子进程
void do_sig(int signo) {
    if (signo == SIGCHLD)
        while (waitpid(-1, NULL, WNOHANG) > 0)
            ;
}

/*
 * 有亲缘关系的进程间的fifo的使用
 * fifo 使用的简单例子
 */
int main(void) {
    int ret;
    int fdr, fdw;
    pid_t pid;

    char words[10] = "123456789";
    char buf[10] = {'\0'};

    // 创建它,若存在则不算是错误,
    // 若想修改其属性需要先打开得到fd,然后用fcntl来获取属性,然后设置属性.

    unlink(FIFO_PATH);
    if (((ret = mkfifo(FIFO_PATH, 0666)) == -1) && (errno != EEXIST))
        // if (((ret = mkfifo(FIFO_PATH, O_CREAT | O_EXCL)) == -1) && (errno != EEXIST))
        sys_err("mkfifo()");
    fprintf(stderr, "fifo : %s created successfully!\n", FIFO_PATH);

    // 回收子进程信号
    signal(SIGCHLD, do_sig);

    pid = fork();
    if (pid == 0) { // child

        if ((fdr = open(FIFO_PATH, O_WRONLY)) < 0) // 打开fifo用来写
            sys_err("open()");
        sleep(2);

        // 写入数据
        if (write(fdr, words, sizeof(words)) != sizeof(words))
            sys_err("write");
        fprintf(stderr, "child write : %s\n", words);
        close(fdw);
    } else if (pid > 0) { // parent

        if ((fdr = open(FIFO_PATH, O_RDONLY)) < 0) // 打开fifo用来读
            sys_err("open()");

        fprintf(stderr, "I father read, waiting for child ...\n");
        if (read(fdr, buf, 9) != 9) //读数据
            sys_err("read");

        fprintf(stderr, "father get buf : %s\n", buf);
        close(fdr);
    }
    // 到这里fifo管道并没有被删除,必须手动调用函数unlink或remove删除.

    return 0;
}
