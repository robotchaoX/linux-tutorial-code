#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct STU {
    int id;
    char name[20];
    char sex;
};

void sys_err(char *str) {
    perror(str);
    exit(1);
}

// mmap无血缘关系进程间通信
int main(int argc, char *argv[]) {
    int fd;
    struct STU student = {10, "xiaoming", 'm'};
    char *mm;

    if (argc < 2) {
        printf("./a.out file_shared\n");
        exit(-1);
    }

    fd = open(argv[1], O_RDWR | O_CREAT, 0664);
    ftruncate(fd, sizeof(student));

    // mmap大小为sizeof(student)
    mm = mmap(NULL, sizeof(student), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mm == MAP_FAILED)
        sys_err("mmap");

    close(fd);

    while (1) {
        memcpy(mm, &student, sizeof(student)); // 每隔一秒覆盖mmap的内容
        student.id++;
        sleep(2);
    }

    munmap(mm, sizeof(student));

    return 0;
}
