#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char **environ; // 引入环境变量表,必须声明环境变量

// 实现getenv函数获取环境变量值
char *my_getenv(const char *name) {
    char *p = NULL;
    int i, len;

    for (i = 0; environ[i] != NULL; i++) {
        p = strstr(environ[i], "=");
        len = p - environ[i];
        if (strncmp(name, environ[i], len) == 0) {
            return p + 1;
        }
    }

    return NULL;
}

// 获取环境变量值
int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("eg: ./getenv SHELL\n");
        exit(1);
    }

    char *p = NULL;

    p = getenv(argv[1]); // 库函数
    // p = my_getenv(argv[1]); // 查询环境变量表 extern char **environ;

    if (p == NULL)
        printf("there is no match\n");
    else
        printf("%s\n", p);

    return 0;
}
