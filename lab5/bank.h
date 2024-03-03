//
// Created by Koschei on 2024/3/3.
//

#ifndef OS_LABS_BANK_H
#define OS_LABS_BANK_H

#include <stdbool.h>

// 系统资源的数量
#define RESOURCES 3
// 系统进程数量
#define PROCESS 5

// 系统可用资源列表
extern int Available[RESOURCES];
// 进程运行所需要的总资源
extern int Max[PROCESS][RESOURCES];
// 进程达到就绪状态还需要的资源
extern int Need[PROCESS][RESOURCES];
// 已经分配给进程的资源
extern int Allocation[PROCESS][RESOURCES];

void init();

void ps();

bool bank(int, int, int[], int[]);

void request(int, const int[]);

#endif //OS_LABS_BANK_H
