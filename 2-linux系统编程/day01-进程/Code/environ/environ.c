#include <stdio.h>

extern char **environ; // 引入环境变量表,必须声明环境变量
//本质：char *envron[] = {"HOME=/home/itcat", "SHELL=/bin/bash", ....}

// 打印当前进程的所有环境变量
int main(void) {
    for (int i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }

    return 0;
}
