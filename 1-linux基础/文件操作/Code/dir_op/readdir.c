#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_file_count(char *root) {
    DIR *dir;
    struct dirent *ptr = NULL;
    int total = 0;
    char path[1024];

    // 打开目录
    dir = opendir(root);
    if (dir == NULL) {
        perror("opendir");
        exit(1);
    }

    // 循环读目录
    while ((ptr = readdir(dir)) != NULL) {
        // 跳过 . 和 ..
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }
        // 判断是不是文件
        if (ptr->d_type == DT_REG) {
            total++;
        }
        // 如果是目录
        if (ptr->d_type == DT_DIR) {
            // 递归读目录
            sprintf(path, "%s/%s", root, ptr->d_name);
            total += get_file_count(path);
        }
    }
    // 关闭目录
    closedir(dir);

    return total;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("./readdir path\n");
        exit(1);
    }

    // 读目录， 统计文件个数
    int total = get_file_count(argv[1]);
    // 打印
    printf("%s has %d files!\n", argv[1], total);

    return 0;
}
