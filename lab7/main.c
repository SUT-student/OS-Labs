//
// Created by Koschei on 2024/3/2.
//

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "page.h"

int main() {
    srand(10 * getpid()); // 进程id作为随机数种子

    int rand_op;

    // 随机生成指令序列
    for (int i = 0; i < total_instructions; i += 5) {
        rand_op = rand() % (total_instructions - 2) + 1; // 1 - 318

        if (rand_op <= 0 || rand_op >= total_instructions - 1) {
            printf("When i == %d, Error, s == %d", i, rand_op);
            exit(0);
        }

        op[i] = rand_op; // 随机选取一个执行起点P
        op[i + 1] = op[i] + 1; // 顺序执行P的下一条指令P+1
        op[i + 2] = rand() % op[i + 1]; // 在前地址随机选取一个指令执行P'

        if (op[i + 2] > 318) {
            printf("op[%d + 2], a number which is: %d and rand_op == %d\n", i, op[i + 2], rand_op);
            exit(0);
        }

        op[i + 3] = op[i + 2] + 1; // 顺序执行P'的下一条指令P'+1
        op[i + 4] = rand() % (total_instructions - op[i + 3] - 1) + op[i + 3] + 1;

        if (op[i + 4] > 319) {
            printf("op[%d + 4], a number which is: %d and rand_op == %d\n", i, op[i + 4], rand_op);
            exit(0);
        }
    }

    printf("输出指令序列:\n");
    for (int i = 0; i < total_instructions; ++i) {
        printf("%3d ", op[i]);
        if ((i + 1) % 20 == 0) puts("");
    }
    printf("\n\n");

    // 指令序列 -> 页地址流
    // 计算指令所属页号和页内偏移量
    for (int i = 0; i < total_instructions; ++i) {
        op2page[i] = op[i] / 10;
        op2offset[i] = op[i] % 10;
    }

    for (int i = 8; i <= MAX_PAGE_FRAMES; ++i) {
        printf("[%2d page frames] 缺页率: ", i);
        FIFO(i);
        LRU(i);
        OPT(i);
        puts("");
    }
}
