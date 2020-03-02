#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// pipe实现 ls | wc -l
// 父进程 ls 结果通过pipe 传给 子进程 wc -l
int main(void) {
    int fd[2]; // pipe读写端  [0]读端 [1]写端

    // 创建管道pipe
    int ret = pipe(fd);
    if (ret == -1) {
        perror("pipe error:");
        exit(1);
    }

    // fork子进程
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork err");
        exit(1);
    } else if (pid == 0) { // child
        close(fd[1]); //子进程从管道中读数据，关闭写端
        dup2(fd[0], STDIN_FILENO); //让wc从管道中读取数据
        execlp("wc", "wc", "-l", NULL); // wc命令默认从标准读入取数据
    } else { // parent
        close(fd[0]); //父进程向管道中写数据，关闭读端
        dup2(fd[1], STDOUT_FILENO); //将ls的结果写入管道中
        execlp("ls", "ls", NULL); // ls输出结果默认对应屏幕
    }

    return 0;
}

/*
  程序不时的会出现先打印$提示符，再出程序运行结果的现象。
  这是因为：父进程执行ls命令，将输出结果给通过管道传递给
  子进程去执行wc命令，这时父进程若先于子进程打印wc运行结果
  之前被shell使用wait函数成功回收，shell就会先于子进程打印
  wc运行结果之前打印$提示符。
  解决方法：让子进程执行ls,父进程执行wc命令。或者在兄弟进程间完成。
 */
