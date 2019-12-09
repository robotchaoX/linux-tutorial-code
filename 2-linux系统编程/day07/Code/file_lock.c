#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void sys_err(char *str) {
    perror(str);
    exit(1);
}

// 多进程间 文件锁：读时共享，写独占
// 多线程中无法使用文件锁，因为各线程之间共享文件描述符（通过文件描述符结构体值加锁）
int main(int argc, char *argv[]) {
    int fd;

    // 定义文件锁
    struct flock f_lock;

    if (argc < 2) {
        printf("./file_lock filename\n");
        exit(1);
    }

    // 打开要加锁的文件
    if ((fd = open(argv[1], O_RDWR)) < 0)
        sys_err("open");

    // 设置文件锁属性，读锁还是写锁
    f_lock.l_type = F_WRLCK; /*选用写琐*/
    //    f_lock.l_type = F_RDLCK;      /*选用读琐*/

    f_lock.l_whence = SEEK_SET; // 文件加锁的位置
    f_lock.l_start = 0;
    f_lock.l_len = 0; /* 0表示整个文件加锁 */

    // 加锁 ，设置文件锁属性
    fcntl(fd, F_SETLKW, &f_lock);
    printf("get flock\n");

    sleep(10);

    // 设置文件锁属性，解锁
    f_lock.l_type = F_UNLCK;
    // 解锁 ，设置文件锁属性
    fcntl(fd, F_SETLKW, &f_lock);
    printf("un flock\n");

    close(fd);

    return 0;
}
