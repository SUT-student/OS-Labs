//
// Created by Koschei on 2024/3/1.
//

#ifndef OS_LABS_SEM_H
#define OS_LABS_SEM_H

#include "../lab1/pcb.h"

// 信号量机制
typedef struct semaphore {
    char name[5]; // 信号量资源名称
    int count; // 计数值
    int cur_pid; // 当前进程ID
    pnode *wait_list;  // 等待进程链表
} semaphore;

void p(char *, pid_t);

void v(char *);

void init();

void print_resources();

#endif //OS_LABS_SEM_H
