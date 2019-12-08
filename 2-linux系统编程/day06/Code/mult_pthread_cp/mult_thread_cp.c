#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define T_NUM 5
#define ITEMS 100 // 显示进度条的长度=个数

void err_sys(void *str) {
    perror(str);
    exit(1);
}

void err_usr(char *str) {
    fputs(str, stderr);
    exit(1);
}

typedef struct {
    int off, size, t_no;
} arg_t;

// 全局变量
char *src_ptr, *dst_ptr;
int *done; //  int done[n]  每个线程完成任务字节数
int n = T_NUM;

// arg{off, size, t_no;}
void *tfn(void *arg) {

    int i;
    char *src_pos_tmp; // 当前线程 src 指针当前位置
    char *dst_pos_tmp; // 当前线程 dst 指针当前位置

    arg_t *arg_p; // typedef struct {} arg_t;
    arg_p = (arg_t *)arg; // 第i个线程等任务内容属性，arg{off, size, t_no;}

    src_pos_tmp = src_ptr + arg_p->off; // 当前线程 src 指针当前位置
    dst_pos_tmp = dst_ptr + arg_p->off; // 当前线程 dst 指针当前位置
    for (i = 0; i < arg_p->size; i++) {
        /* 逗号表达式的使用技巧*/
        *dst_pos_tmp++ = *src_pos_tmp++, done[arg_p->t_no]++; // 每次复制一个字节，复制后完成任务字节数done[i]加一
        usleep(10);
    }

    return NULL;
}

// 显示进度线程
void *display(void *arg) {
    int size, interval, draw, sum, i, j;

    size = (int)arg; // 源文件总大小
    // 每复制完一个interval字节大小，对应显示一个=号
    interval = size / (ITEMS - 1); // ITEMS 显示进度条的长度=总个数
    draw = 0; // 已经显示的=个数
    while (draw < ITEMS) {
        // sum 所有线程完成任务的字节数累加
        for (i = 0, sum = 0; i < n; i++) {
            sum += done[i]; // done[i] 第i个线程完成任务字节数
        }
        j = sum / interval + 1; // 需要显示的=个数
        for (; j > draw; draw++) {
            putchar('=');
            fflush(stdout); // 刷新缓冲区输出
        }
    }
    putchar('\n');

    return NULL;
}

int main(int argc, char *argv[]) {
    int src, dst, i, len, off;
    struct stat statbuf;

    pthread_t *tid; // pthread_t tid[n+1];

    arg_t *arr; // arr[n] 每个线程的任务 // typedef struct

    if (argc != 3 && argc != 4)
        err_usr("usage : ./mult_thread_cp src.txt dst.txt [thread_no]\n");
    if (argc == 4)
        n = atoi(argv[3]); // n 设置的线程数目

    // 打开源文件
    src = open(argv[1], O_RDONLY);
    if (src == -1)
        err_sys("fail to open");
    // 打开目的文件
    dst = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (dst == -1)
        err_sys("fail to open");
    // 源文件大小属性
    if (fstat(src, &statbuf) == -1)
        err_sys("fail to stat");
    // lseek拓展目的文件与源文件大小相同，也可以使用truncate
    lseek(dst, statbuf.st_size - 1, SEEK_SET);
    write(dst, "a", 1); // 必须写一下才行 // IO操作拓展文件大小,也可以使用truncate
    // int dst_size = ftruncate(dst, statbuf.st_size); // 也可以使用lseek
    // if (dst_size == -1) {
    // err_sys("fail to truncate");
    // }

    // 创建内存映射区
    // src mmap
    src_ptr = (char *)mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, src, 0);
    if (src_ptr == MAP_FAILED)
        err_sys("fail to mmap");
    // dst mmap
    dst_ptr = (char *)mmap(NULL, statbuf.st_size, PROT_WRITE, MAP_SHARED, dst, 0); // 同步到文件
    if (dst_ptr == MAP_FAILED)
        err_sys("fail to mmap");
    // 关闭打开的文件
    close(src);
    close(dst);

    // n 设置同时工作的线程数目
    // pthread_t tid[n+1];
    tid = (pthread_t *)malloc(sizeof(pthread_t) * (n + 1));
    if (tid == NULL)
        err_sys("fail to malloc");
    // int done[n]  每个线程完成任务字节数
    done = (int *)calloc(sizeof(int), n); // 都是从堆分配内存，calloc分配的空间初始化为0
    // alloc函数适合为数组申请空间，可以将size设置为数组元素的空间长度，将n设置为数组的容量。
    if (done == NULL)
        err_sys("fail to malloc");
    // arr[n] 每个线程的任务
    arr = (arg_t *)malloc(sizeof(arg_t) * n); // 在堆上开辟空间，malloc只分配空间不初始化
    if (arr == NULL)
        err_sys("fail to malloc");

    //构建线程任务数组，分配任务
    len = statbuf.st_size / n; // 每个线程分配的总任务len
    off = 0; // off 偏移，每个线程的起始复制地址
    // arr[i].off 第i个线程的起始复制地址，arr[i].size第i个线程的总任务，arr[i].t_no第i个线程的序号
    for (i = 0; i < n; i++, off += len)
        arr[i].off = off, arr[i].size = len, arr[i].t_no = i;
    arr[n - 1].size += (statbuf.st_size % n); // 不能整除剩余的部分，由最后一个线程复制

    // 创建执行拷贝任务线程
    for (i = 0; i < n; i++)
        pthread_create(&tid[i], NULL, tfn, (void *)&arr[i]); // arr[i] 对应第i个线程的任务

    // 创建显示进度线程
    pthread_create(&tid[n], NULL, display, (void *)statbuf.st_size);

    // 回收所有子线程
    for (i = 0; i < n + 1; i++)
        pthread_join(tid[i], NULL);

#if 1
    // 删除映射区
    munmap(src_ptr, statbuf.st_size);
    munmap(dst_ptr, statbuf.st_size);
#endif
    free(tid);
    free(done);
    free(arr);

    return 0;
}
