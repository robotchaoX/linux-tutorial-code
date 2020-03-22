#include <stdio.h>

// 内置变量
// __func__    main
// __FILE__    builtin_var.c
int main(void) {
    printf("%s\n%s\n", __func__, __FILE__);

    return 0;
}
