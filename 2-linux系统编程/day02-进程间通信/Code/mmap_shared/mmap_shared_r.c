#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

struct STU {
    int id;
    char name[20];
    char sex;
};

// strace 监控用户空间进程和内核的交互

void sys_err(char *str) {
    perror(str);
    exit(-1);
}

// mmap无血缘关系进程间通信
int main(int argc, char *argv[]) {
    int fd;
    struct STU student;
    struct STU *mm;

    if (argc < 2) {
        printf("./a.out file_shared\n");
        exit(-1);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
        sys_err("open error");

    // mmap大小为sizeof(student)
    mm = mmap(NULL, sizeof(student), PROT_READ, MAP_SHARED, fd, 0);
    if (mm == MAP_FAILED)
        sys_err("mmap error");

    close(fd);

    // 可以重复读取
    while (1) {
        printf("id=%d\tname=%s\t%c\n", mm->id, mm->name, mm->sex); // 每隔一秒读mmap的内容
        sleep(1);
    }

    munmap(mm, sizeof(student));

    return 0;
}
