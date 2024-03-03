//
// Created by Koschei on 2024/3/3.
//

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include "bank.h"

// 系统可用资源列表
int Available[RESOURCES] = {3, 3, 2};
// 进程运行所需要的总资源
int Max[PROCESS][RESOURCES] = {{7, 5, 3},
                               {3, 2, 2},
                               {9, 0, 2},
                               {2, 2, 2},
                               {4, 3, 3}};
// 进程达到就绪状态还需要的资源
int Need[PROCESS][RESOURCES] = {{7, 4, 3},
                                {1, 2, 2},
                                {6, 0, 0},
                                {0, 1, 1},
                                {4, 3, 1}};
// 已经分配给进程的资源
int Allocation[PROCESS][RESOURCES] = {{0, 1, 0},
                                      {2, 0, 0},
                                      {3, 0, 2},
                                      {2, 1, 1},
                                      {0, 0, 2}};

void init() {
    // 随机产生资源和使用情况
//    srand(10 * getpid());
//    for (int i = 0; i < PROCESS; i++) {
//        for (int j = 0; j < RESOURCES; j++) {
//            Available[j] = rand() % 5 + 15;
//            Max[i][j] = rand() % 7 + 1;
//            Allocation[i][j] = rand() % 3 + 1;
//            Need[i][j] = Max[i][j] - Allocation[i][j];
//        }
//    }
}

// 查看当前账本
void ps() {
    printf("\n");

    for (int i = 0; i < 9 + 2 * 3 * RESOURCES + RESOURCES % 2 + 3; ++i) {
        printf("=");
    }

    printf("\nAvailable:");
    for (int i = 0; i < RESOURCES; i++) {
        printf("%3d ", Available[i]);
    }
    printf("\nProcess |");
    for (int i = 0; i < (int) ((3 * RESOURCES + 2) / 2.0 + 0.5) - 5; ++i) {
        printf(" ");
    }
    printf("Allocation");
    for (int i = 0; i < (int) ((3 * RESOURCES + 2) / 2.0 + 0.5) - 5; ++i) {
        printf(" ");
    }
    printf("|");

    for (int i = 0; i < (int) ((3 * RESOURCES + 2) / 2.0 + 0.5) - 2; ++i) {
        printf(" ");
    }
    printf("Need\n");

    // Process
    for (int i = 0; i < PROCESS; i++) {
        printf("%7d |", i);
        // Allocation
        for (int j = 0; j < RESOURCES; j++) {
            printf("%3d", Allocation[i][j]);
        }
        if (RESOURCES % 2) {
            printf(" ");
        }
        printf("  |");
        // Need
        for (int k = 0; k < RESOURCES; k++) {
            printf("%3d", Need[i][k]);
        }
        printf("\n");
    }

    for (int i = 0; i < 9 + 2 * 3 * RESOURCES + RESOURCES % 2 + 3; ++i) {
        printf("=");
    }

    printf("\n\n");
}

/**
 * 银行家算法
 * pid：当前要运行的进程
 * no：当前运行的进程顺序
 * work：当前系统可用资源
 * finish：记录进程完成状态和顺序
 **/
bool bank(int pid, int no, int work[], int finish[]) {
    printf("新一轮运行，尝试的pid: %d，no: %d\n", pid, no);

    // 判断当前系统资源能否满足当前进程运行
    for (int i = 0; i < RESOURCES; i++) {
        // 不满足运行条件，终止本次尝试，向上回溯
        if (work[i] < Need[pid][i]) {
            printf("当前pid: %d, 可用的资源: %d, 需要的资源: %d。不满足运行条件！\n", pid, work[i], Need[pid][i]);
            return false;
        }
    }

    // 当前资源满足了当前进程运行
    // 标记当前进程已完成
    finish[pid] = no;

    // 释放资源
    for (int i = 0; i < RESOURCES; i++) {
        work[i] += Allocation[pid][i];
    }

    printf("当前进程成功运行完成: %d, 是第 %d 个完成的进程。寻找下一个可运行的进程...\n", pid, no);

    // 判断系统是否运行完成
    if (no >= PROCESS) {
        printf("分配完毕，系统安全！安全序列：");
        for (int i = 1; i <= PROCESS; i++) {
            for (int j = 0; j < PROCESS; j++) {
                if (finish[j] == i) {
                    printf("%d -> ", j);
                }
            }
        }
        printf("finish\n");
        return true;
    }

    // 寻找下一个未完成的进程
    for (int i = 0; i < PROCESS; i++) {
        if (!finish[i]) {
            bool flag = bank(i, no + 1, work, finish);
            if (flag) return true;
        }
    }

    // 没有下一个可以运行的进程了，系统进入不安全状态，往上回溯
    for (int i = 0; i < RESOURCES; i++) {
        work[i] -= Allocation[pid][i];
    }
    finish[pid] = 0;

    printf("没有下一个可以运行的进程了，系统进入不安全状态，往上回溯。当前进程pid: %d, no: %d\n", pid, no);
    return false;
}

/**
 * 申请使用资源
 * pid：申请资源的进程ID
 * req_list：申请的资源列表
 **/
void request(int pid, const int req_list[]) {
    // pid校验
    if (pid >= PROCESS) {
        printf("进程ID非法！\n");
        return;
    }

    // req_list校验
    for (int i = 0; i < RESOURCES; i++) {
        if (req_list[i] > Need[pid][i]) {
            printf("请求了过多的资源，分配终止！\n");
            return;
        }
        if (req_list[i] > Available[i]) {
            printf("系统可用资源不足，无法分配！\n");
            return;
        }
    }

    // 使用银行家算法判断系统是否安全
    int work[RESOURCES], finish[PROCESS];
    memset(finish, 0, sizeof finish);

    // 试探性分配，然后检查是否安全
    for (int i = 0; i < RESOURCES; i++) {
        Available[i] -= req_list[i];
        Allocation[pid][i] += req_list[i];
        Need[pid][i] -= req_list[i];
        work[i] = Available[i];
    }

    // 检查是否安全
    bool safe = false;
    for (int i = 0; i < PROCESS; i++) {
        if (bank(i, 1, work, finish)) {
            safe = true;
            break;
        }
    }

    // 如果系统安全，则本次分配完成
    if (safe) {
        printf("本次分配完成！\n");
        ps();
    } else {
        // 分配不安全，本次分配作废
        for (int i = 0; i < RESOURCES; i++) {
            Available[i] += req_list[i];
            Allocation[pid][i] -= req_list[i];
            Need[pid][i] += req_list[i];
        }
        printf("没有安全的分配方案，请求资源失败！\n");
    }
}
