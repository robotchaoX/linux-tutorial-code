#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * FIFO server
 */

// todo
int main(void) {
    int fdw, fdw2;
    int fdr;
    char clt_path[PATH_LEN] = {'\0'};
    char buf[MAX_LINE] = {'\0'};
    char *p;
    int n;

    if (mkfifo(FIFO_SVR, FILE_MODE) == -1 && errno != EEXIST)
        perr_exit("mkfifo()");
    if ((fdr = open(FIFO_SVR, O_RDONLY)) < 0)
        perr_exit("open()");
    /*
     * 根据fifo的创建规则, 若从一个空管道或fifo读,

     * 而在读之前管道或fifo有打开来写的操作, 那么读操作将会阻塞
     * 直到管道或fifo不打开来读, 或管道或fifo中有数据为止.

     *

     * 这里,我们的fifo本来是打开用来读的,但是为了,read不返回0,

     * 让每次client端读完都阻塞在fifo上,我们又打开一次来读.
     * 见unpv2 charper 4.7
     */
    if ((fdw2 = open(FIFO_SVR, O_WRONLY)) < 0)
        fprintf(stderr, "open()");

    while (1) {
        /* read client fifo path from FIFO_SVR */

        /* 这里由于FIFO_SVR有打开来写的操作,所以当管道没有数据时,

         * read会阻塞,而不是返回0.

         */
        if (read(fdr, clt_path, PATH_LEN) < 0) {
            fprintf(stderr, "read fifo client path error : %s\n", strerror(errno));
            break;
        }
        if ((p = strstr(clt_path, "\r\n")) == NULL) {
            fprintf(stderr, "clt_path error: %s\n", clt_path);
            break;
        }
        *p = '\0';
        DBG("clt_path", clt_path);
        if (access(clt_path, W_OK) == -1) { // client fifo ok, but no permission

            perror("access()");
            continue;
        }
        /* open client fifo for write */
        if ((fdw = open(clt_path, O_WRONLY)) < 0) {
            perror("open()");
            continue;
        }
        if ((n = read(fdr, buf, WORDS_LEN)) > 0) { /* read server words is ok */
            printf("server read words : %s\n", buf);
            buf[n] = '\0';
            write(fdw, buf, strlen(buf));
        }
    }

    close(fdw);
    unlink(FIFO_SVR);
    exit(0);
}