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
 * Fifo client
 *
 */

// todo
int main(void) {
    int fdr, fdw;
    pid_t pid;
    char clt_path[PATH_LEN] = {'\0'};
    char buf[MAX_LINE] = {'\0'};
    char buf_path[MAX_LINE] = {'\0'};

    snprintf(clt_path, PATH_LEN, FIFO_CLT_FMT, (long)getpid());
    DBG("clt_path1 = ", clt_path);
    snprintf(buf_path, PATH_LEN, "%s\r\n", clt_path);

    if (mkfifo(clt_path, FILE_MODE) == -1 && errno != EEXIST)
        perr_exit("mkfifo()");

    /* client open clt_path for read
     * open server for write
       */
    if ((fdw = open(FIFO_SVR, O_WRONLY)) < 0)
        perr_exit("open()");

    /* write my fifo path to server */
    if (write(fdw, buf_path, PATH_LEN) != PATH_LEN)
        perr_exit("write()");
    if (write(fdw, WORDS, WORDS_LEN) < 0) /* write words to fifo server */
        perr_exit("error");

    if ((fdr = open(clt_path, O_RDONLY)) < 0)
        perr_exit("open()");
    if (read(fdr, buf, WORDS_LEN) > 0) { /* read reply from fifo server */
        buf[WORDS_LEN] = '\0';
        printf("server said : %s\n", buf);
    }

    close(fdr);
    unlink(clt_path);

    exit(0);
}