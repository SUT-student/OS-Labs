//
// Created by Koschei on 2024/3/2.
//

#ifndef OS_LABS_DISPATCHER_H
#define OS_LABS_DISPATCHER_H

#include <string.h>
#include "../lab1/pcb.h"

// 定义进程数
# define PROCESS_NUMBER 30
// 定义最高优先级（0-8）
#define MAX_PRIORITY 9
// 最大进程生命
#define MAX_LIFE 50

typedef struct priority_node priority_node;

// 优先级队列
struct priority_node {
    pcb *pcb;
    priority_node *next;
};

extern int life;    // 系统剩余总生命周期
extern char command[9][10];    // 命令列表
extern priority_node *priority_array[MAX_PRIORITY]; // 优先级队列数组

void ps();

void init();

void create();

void kill();

int runtime();

void run();

void update_pri();

int allocate_pid();

#endif //OS_LABS_DISPATCHER_H
