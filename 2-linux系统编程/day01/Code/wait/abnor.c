/*测试用例,用以验证wait异常情况*/

int main(void) {
    char *p = "test of wait abnormally\n";

    //    p[0] = 'h'; // 段错误
    int a = 5 / 0; // 除0

    return 56;
}
