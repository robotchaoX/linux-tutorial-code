#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

// linux系统 建立匿名映射
int main(void) {
    int *p;
    pid_t pid;

    // 匿名映射,不需要打开文件，用linux宏定义 MAP_ANONYMOUS 权限 ，文件描述符 -1
    p = mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); // MAP_ANONYMOUS 或 MAP_ANON
    // p = mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); // 父子进程各自独占映射区
    if (p == MAP_FAILED) { //注意:不是p == NULL
        perror("mmap error");
        exit(1);
    }

    *p = 11; // mmap空间赋值数据
    pid = fork(); //创建子进程
    if (pid == 0) {
        *p = 2000;
        printf("child, *p = %d\n", *p);
    } else {
        sleep(1);
        printf("parent, *p = %d\n", *p);
    }

    munmap(p, 4); //释放映射区

    return 0;
}
