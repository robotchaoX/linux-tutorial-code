#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static const int NBUFF = 10000;
static const int MAXNTHREADS = 100;
static int nitems; //总共生产的条目数
static int buff[NBUFF]; //生产者向其中放数据，消费者从中取数据

static struct put //生产者使用的结构，向其中互斥的放数据
{
    pthread_mutex_t mutex;
    int nput; // net position to put
    int nval; // next value to store
} put = {PTHREAD_MUTEX_INITIALIZER};

//记录缓冲区的状态，准备好的数目，消费者唯一关注的结构，当然生产者也会使用
static struct nready {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int nget;
    int nready; // number ready for consumer
} nready = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER}; //

void *produce(void *);
void *consume(void *);

int main(int argc, char **argv) {
    if (argc != 4) {
        err_quit("Usage: a.out <#items> <#produce_nthreads>
                  <#consume_nthreads>");
    }
    nitems = atoi(argv[1]);
    int produce_nthreads = min(atoi(argv[2]), MAXNTHREADS);
    int consume_nthreads = min(atoi(argv[3]), MAXNTHREADS);

    // Solaris 2.6需要设置线程并发数
    // Set_concurrency(nthreads + 1);

    pthread_t tid_produce[MAXNTHREADS];
    for (int i = 0; i < produce_nthreads; ++i) {
        Pthread_create(&tid_produce[i], NULL, produce, NULL);
    }
    pthread_t tid_consume[MAXNTHREADS];
    for (int i = 0; i < consume_nthreads; ++i) {
        Pthread_create(&tid_consume[i], NULL, consume, NULL);
    }

    //等待线程终止
    for (int i = 0; i < produce_nthreads; ++i) {
        Pthread_join(tid_produce[i], NULL);
    }
    for (int i = 0; i < consume_nthreads; ++i) {
        Pthread_join(tid_consume[i], NULL);
    }

    exit(0);
}

void *produce(void *arg) {
    printf("producd\n");

    //多个生产者
    for (;;) {
        Pthread_mutex_lock(&put.mutex);
        //已存了需要多的数
        if (put.nval >= nitems) {
            Pthread_mutex_unlock(&put.mutex);
            return NULL;
        }
        buff[put.nput] = put.nval;
        if (++put.nput >= NBUFF) {
            put.nput = 0;
        }
        ++put.nval;
        Pthread_mutex_unlock(&put.mutex);

        //当生产了数据后通知条件变量，应该使临界区尽量短，宁愿使用多个互斥量
        Pthread_mutex_lock(&nready.mutex);
        if (nready.nready == 0) {
            Pthread_cond_signal(&nready.cond);
        }
        ++nready.nready;
        Pthread_mutex_unlock(&nready.mutex);
    } // end for(;;)
    return NULL;
}

void *consume(void *argv) {
    printf("consume\n");

    //多个消费者
    //只生产nitems个选项
    for (;;) {
        Pthread_mutex_lock(&nready.mutex);
        // while避免虚假唤醒
        while (nready.nready == 0) {
            Pthread_cond_wait(&nready.cond, &nready.mutex);
        }
        // int ival = buff[nready.nget];
        // if (++nready.nget == NBUFF) {
        //    nready.nget = 0;
        //}
        if (++nready.nget >= nitems) {
            // nget比较的取值为1..nitems，当为nitems时少操作了一次，总共操作nitems次
            if (nready.nget == nitems) {
                printf("buff[%d] = %d\n", nready.nget - 1, buff[(nready.nget - 1) % NBUFF]);
            }
            Pthread_cond_signal(&nready.cond);
            Pthread_mutex_unlock(&nready.mutex);
            return NULL;
        }
        --nready.nready;
        Pthread_mutex_unlock(&nready.mutex);

        //仅仅读数据不许要互斥
        // if (buff[nready.nget - 1] != nready.nget - 1)
        {
            printf("buff[%d] = %d\n", nready.nget - 1, buff[(nready.nget - 1) % NBUFF]);
            // printf("buff[%d] = %d\n", nready.nget,
                      buff[nready.nget]);
                      //}
        } // end for(i:0..nitems)
        return NULL;
    }