#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

// 类Unix系统 建立匿名映射
int main(void) {
    int *p;
    pid_t pid;

    int fd;
    fd = open("/dev/zero", O_RDWR); // 想要多大有多大
    // 匿名映射
    p = mmap(NULL, 400, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // MAP_SHARED(共享映射) 父子进程共享映射区
    // p = mmap(NULL, 400, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0); //MAP_PRIVATE(私有映射) 父子进程各自独占映射区

    if (p == MAP_FAILED) { //注意:不是p == NULL
        perror("mmap error");
        exit(1);
    }

    *p = 11; // mmap空间赋值
    pid = fork(); //创建子进程
    if (pid == 0) { // 子进程
        *p = 2000;
        printf("child, *p = %d\n", *p);
    } else { // 父进程
        sleep(1);
        printf("parent, *p = %d\n", *p);
    }

    munmap(p, 4); //释放映射区

    return 0;
}
