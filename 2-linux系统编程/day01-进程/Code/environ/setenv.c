#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *val;
    const char *name = "ABD";

    val = getenv(name);
    printf("1, %s = %s\n", name, val);
    // 设置新增环境变量
    setenv(name, "haha-day-and-night", 1);

    // 获取环境变量
    val = getenv(name);
    printf("2, %s = %s\n", name, val);

    // 删除环境变量
    int ret = unsetenv("ABD"); // name=value:value
    printf("ret = %d\n", ret);

    val = getenv(name);
    printf("3, %s = %s\n", name, val);

    return 0;
}
