#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

// 全局变量进程独享
int var = 111;

// 父进程创建映射区，然后fork子进程，子进程修改映射区内容，而后，父进程读取映射区内容，查验是否共享
int main(void) {
    int *p;
    pid_t pid;

    int fd;
    fd = open("temp", O_RDWR | O_CREAT | O_TRUNC, 0644); // 这个文件的作用只是用于创建mmap
    if (fd < 0) {
        perror("open error");
        exit(1);
    }
    unlink("temp"); //删除临时文件目录项,使之具备被释放条件.
    ftruncate(fd, 4); // 拓展文件大小

    p = (int *)mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // 父子进程共享mmap，父子间相同同，能够用于通信
    // p = (int *)mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0); // 父子进程独享mmap，不能用于通信
    if (p == MAP_FAILED) { //注意:不是p == NULL
        perror("mmap error");
        exit(1);
    }
    close(fd); //映射区建立完毕,即可关闭文件

    pid = fork(); //创建子进程
    if (pid == 0) { // 子进程
        *p = 2020; // mmap
        var = 222; // 全局变量
        printf("child, *p = %d, var = %d\n", *p, var);
    } else { //父进程
        sleep(1);
        printf("parent, *p = %d, var = %d\n", *p, var);
        wait(NULL); // 回收子进程

        int ret = munmap(p, 4); //释放映射区
        if (ret == -1) {
            perror("munmap error");
            exit(1);
        }
    }

    return 0;
}
