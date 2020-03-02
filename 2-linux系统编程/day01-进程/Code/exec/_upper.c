#include <ctype.h>
#include <stdio.h>

// 键盘输入小写字符,屏幕输出对应大写
int main(void) {
    int ch;

    while ((ch = getchar()) != EOF) {
        putchar(toupper(ch));
    }

    return 0;
}
