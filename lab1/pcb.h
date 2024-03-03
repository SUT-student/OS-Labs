//
// Created by Koschei on 2024/2/29.
//

#ifndef OS_LABS_PCB_H
#define OS_LABS_PCB_H

#include <unistd.h>

// 定义进程状态枚举类型
typedef enum ProcessState {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} p_state;

typedef struct PNODE pnode;

// 基础进程控制块（PCB）
typedef struct PCB {
    int pid; // 进程id
    int ppid;// 父进程id
    int uid; // 用户id
    int pri; // 进程优先级
    p_state state; // 进程状态
    // 上次运行时间
    int last_time;
    // 进程运行总时间
    int totaltime;
    int life;    // 剩余生命周期
} pcb;

// 进程组织结构：进程在广义二叉树或者进程链表中的节点结构
struct PNODE {
    // 当前节点对应的进程控制块
    pcb *pcb;
    // 进程树中，当前节点的左右子节点
    pnode *left;
    pnode *right;
    // 进程树中，当前节点的兄弟节点
    pnode *brother;
    // 进程链表中，当前节点的下一节点
    pnode *next;
};

// 进程树根节点
extern pnode *root;
// 进程链表头节点
extern pnode *plink;

void initRoot();

void print_tree(pnode *, int);

int createPCB(pid_t pid, pid_t ppid, int pri);

#endif //OS_LABS_PCB_H
