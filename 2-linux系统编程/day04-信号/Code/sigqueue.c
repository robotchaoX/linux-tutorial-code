#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#if 0

// 传出参数
siginfo_t {
    int      si_signo;    /* Signal number */
    int      si_errno;    /* An errno value */
    int      si_code;     /* Signal code */
    int      si_trapno;   /* Trap number that caused
                             hardware-generated signal
                             (unused on most architectures) */
    pid_t    si_pid;      /* Sending process ID */
    uid_t    si_uid;      /* Real user ID of sending process */
    int      si_status;   /* Exit value or signal */
    clock_t  si_utime;    /* User time consumed */
    clock_t  si_stime;    /* System time consumed */
    sigval_t si_value;    /* Signal value */
    int      si_int;      /* POSIX.1b signal */
    void    *si_ptr;      /* POSIX.1b signal */
    int      si_overrun;  /* Timer overrun count; POSIX.1b timers */
    int      si_timerid;  /* Timer ID; POSIX.1b timers */
    void    *si_addr;     /* Memory location which caused fault */
    long     si_band;     /* Band event (was int in
                             glibc 2.3.2 and earlier) */
    int      si_fd;       /* File descriptor */
    short    si_addr_lsb; /* Least significant bit of address
                             (since Linux 2.6.32) */
}

union sigval {
   int   sival_int; // 整数
   void *sival_ptr; // 地址,不同进程之间虚拟地址空间各自独立，将当前进程地址传递给另一进程没有实际意义。
};

#endif

// signo 信号编号， uinfo 传出参数，
void func(int signo, siginfo_t *uinfo, void *val) {
    printf("catch signal SIGINT %d\n", signo);

    printf("-----------signo is = %d\n", uinfo->si_signo);
    printf("-----------send pid = %u\n", uinfo->si_pid);
    printf("-----------sender's value is %d\n", uinfo->si_value); // 传递的参数 union sigval 的值
    printf("----------- value is %s\n", (char *)uinfo->si_ptr); // 传递的参数 union sigval 的值

    return;
}

// sigqueue函数对应kill函数，但可在向指定进程发送信号的同时携带参数
int main(void) {
    pid_t pid = fork();

    // 定义参数，值被存入siginfo_t
    union sigval uval; // 系统 union sigval
    uval.sival_int = 887; // 传递的参数值
    uval.sival_ptr = "abc"; // 传递的参数值

    if (pid > 0) { // 父进程
        // int sigqueue(pid_t pid, int sig, const union sigval value);
        sleep(1);
        // 父进程发送信号
        // sigqueue 向指定进程发送信号的同时携带参数，类似kill
        sigqueue(pid, SIGUSR1, uval); // 类似 kill 给子进程发信号 ,同时携带数据
        // 回收子进程
        wait(NULL);

    } else if (pid == 0) { // 子进程
        int n = 5;

        struct sigaction act;
        // void  (*sa_sigaction)(int, siginfo_t *, void *);
        act.sa_sigaction = func;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_SIGINFO; // SA_SIGINFO 选用 sigaction 来指定捕捉函数
        // 子线程捕获信号
        sigaction(SIGUSR1, &act, NULL);

        while (n--) {
            printf("I'm child pid = %d\n", getpid());
            sleep(1);
        }
    }

    return 0;
}
