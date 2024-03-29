//
// Created by Koschei on 2024/3/4.
//

#include <string.h>
#include <stdio.h>
#include "memory.h"

/**
 * 控制台
 **/
void terminal() {
    char cmdstr[32], fit_algorithm[20];
    int pid, alloc_size, start_addr;
    while (1) {
        printf("cmd: ");
        scanf("%s", cmdstr);

        if (strcmp(cmdstr, "exit") == 0) {
            return;
        }

        if (strcmp(cmdstr, "help") == 0) {
            printf("\n=================================================\n");
            printf("申请内存: allocate\n");
            printf("释放内存: release\n");
            printf("查看当前空余内存: ps\n");
            printf("获取帮助: help\n");
            printf("退出: exit\n");
            printf("=================================================\n\n");
            continue;
        }

        if (strcmp(cmdstr, "ps") == 0) {
            ps();
            continue;
        }

        if (strcmp(cmdstr, "allocate") == 0) {
            printf("要申请的进程号: ");
            scanf("%d", &pid);
            printf("要申请的内存大小(MB): ");
            scanf("%d", &alloc_size);
            printf("要使用的动态内存分配算法[first:首次适应算法; next:循环首次适应算法; best:最佳适应算法]: ");
            scanf("%s", fit_algorithm);
            if (strcmp(fit_algorithm, "first") == 0) {
                allocate(pid, alloc_size, first_fit);
            } else if (strcmp(fit_algorithm, "next") == 0) {
                allocate(pid, alloc_size, next_fit);
            } else if (strcmp(fit_algorithm, "best") == 0) {
                allocate(pid, alloc_size, best_fit);
            } else {
                printf("ERROR: 参数错误!");
            }
            continue;
        }

        if (strcmp(cmdstr, "release") == 0) {
            printf("要释放内存起始地址: ");
            scanf("%d", &start_addr);
            release(start_addr);
            continue;
        }

        printf("cmd: 未知的命令！\n");
    }
}

int main() {
    init();
    ps();
    terminal();
    return 0;
}
