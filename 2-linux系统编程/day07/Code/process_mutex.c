#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

struct mt { // 捆绑 数据 锁 属性，只是为了方便
    int num; // 共享数据
    pthread_mutex_t mutex; // 定义mutex锁
    pthread_mutexattr_t mutexattr; // 定义mutex锁的属性
};

// mutex默认线程锁，可以修改属性变为进程锁
int main(void) {
    int i;
    struct mt *mm; // 局部变量
    pid_t pid;
    /*
    // 创建实名映射区
        int fd = open("mt_test", O_CREAT | O_RDWR, 0777);
        ftruncate(fd, sizeof(*mm));
        mm = mmap(NULL, sizeof(*mm), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        close(fd);
        unlink("mt_test"); // 临时文件
    */
    // 创建匿名映射区
    // 局部变量接收mmap返回值，所以 mm 父子进程是共享的
    mm = mmap(NULL, sizeof(*mm), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    memset(mm, 0, sizeof(*mm)); // 内存空间初始化，清0

    // 初始化锁属性
    pthread_mutexattr_init(&mm->mutexattr); //初始化mutex属性对象
    // 修改锁属性
    pthread_mutexattr_setpshared(&mm->mutexattr, PTHREAD_PROCESS_SHARED); //修改属性为进程间共享

    // 初始化锁
    pthread_mutex_init(&mm->mutex, &mm->mutexattr); //初始化一把mutex琐

    // 创建进程
    pid = fork();
    if (pid == 0) { // 子进程
        for (i = 0; i < 10; i++) { // 循环10次
            // 加锁
            pthread_mutex_lock(&mm->mutex);
            (mm->num)++; // 修改共享数据，mm 父子进程是共享的，来自mmap
            printf("-child----------num++   %d\n", mm->num);
            // 解锁
            pthread_mutex_unlock(&mm->mutex);
            usleep(50000);
        }
    } else if (pid > 0) { // 父进程
        for (i = 0; i < 10; i++) { // 循环10次
            //    sleep(1);
            // 加锁
            pthread_mutex_lock(&mm->mutex);
            mm->num += 2; // 修改共享数据，mm 父子进程是共享的，来自mmap
            printf("-------parent---num+=2  %d\n", mm->num);
            // 解锁
            pthread_mutex_unlock(&mm->mutex);
            usleep(50000);
        }

        // 父进程阻塞回收子进程
        wait(NULL);
    }

    pthread_mutexattr_destroy(&mm->mutexattr); //销毁mutex属性对象
    pthread_mutex_destroy(&mm->mutex); //销毁mutex
    munmap(mm, sizeof(*mm)); //释放映射区

    return 0;
}
