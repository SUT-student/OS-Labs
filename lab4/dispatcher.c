//
// Created by Koschei on 2024/3/2.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "dispatcher.h"

int life;    // 系统剩余总生命周期
int sys_time; // 系统过去的时间
char command[9][10];    // 命令列表
static int id_list[PROCESS_NUMBER];    // 进程ID列表，记录进程创建状态
priority_node *priority_array[MAX_PRIORITY]; // 优先级队列数组

void Log(const char *format, ...) {
    // 打开文件流，以追加方式写入文件
    FILE *fd = fopen("output.txt", "a");
    if (fd == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return;
    }

    // 声明变量存储参数列表
    va_list args;
    // 初始化args为参数列表的起始位置
    va_start(args, format);

    // 将格式化输出发送到标准输出
    vprintf(format, args);

    // 将格式化输出发送到文件流
    vfprintf(fd, format, args);

    // 结束参数列表的使用
    va_end(args);

    // 关闭文件流
    fclose(fd);
}

/**
 * 系统初始化
 **/
void init() {
    srand(10 * getpid());

    sys_time = 0;
    for (int i = 0; i < MAX_PRIORITY; i++) {
        priority_array[i] = NULL;
    }
    strcpy(command[0], "exit");
    strcpy(command[1], "help");
    strcpy(command[2], "ps");
    strcpy(command[3], "create");
    strcpy(command[4], "kill");
    strcpy(command[5], "runtime");
    strcpy(command[6], "sleep");
    strcpy(command[7], "awake");
    strcpy(command[8], "run");

    // 打开文件以写入模式，如果文件存在则清除
    FILE *fd = fopen("output.txt", "w+");
    fclose(fd);
}

int allocate_pid() {
    int i = 0;
    for (; i < PROCESS_NUMBER && id_list[i] != 0; ++i);
    if (i < PROCESS_NUMBER) {
        id_list[i] = 1;
        return i;
    }
    return -1;
}

/**
 * 输出系统当前进程状态
 * p: 遍历指针
 **/
void ps() {
    priority_node *p;
    Log("\n=================================================\n");
    Log("Total system life: %d\n", life);
    for (int i = 0; i < MAX_PRIORITY; i++) {
        Log("Priority level: %d\n", i);
        p = priority_array[i];
        while (p != NULL) {
            Log("id: %d, status: %d, priority: %d, life: %d\n", p->pcb->pid, p->pcb->state, p->pcb->pri,
                p->pcb->life);
            p = p->next;
        }
    }
    Log("=================================================\n\n");
}

/**
 * 随机创建一个新进程
 * pid：进程ID
 * pri：进程优先级
 * life：进程生命周期
 * p_head：指向同等优先级链表表头，遍历中发生变化
 * p：指向新创建的进程
 **/
void create() {
    // 创建新进程
    pcb *p = (pcb *) malloc(sizeof(pcb));
    p->pid = allocate_pid();

    // 检查是否到达进程数量上限，同时分配进程ID
    if (p->pid == -1) {
        Log("System max process error!\n");
        return;
    }

    p->pri = rand() % MAX_PRIORITY;
    p->life = rand() % MAX_LIFE + 1;

    // 假设进程创建完就已就绪
    p->state = READY;

    // 新进程创建时间为sys_time
    p->last_time = sys_time;

    // 增加总生命周期
    life += p->life;

    priority_node *p_node = (priority_node *) malloc(sizeof(priority_node));
    p_node->pcb = p;
    p_node->next = NULL;

    // 插入到进程优先级队列中
    priority_node *p_head = priority_array[p->pri];
    // 如果表头为NULL，新进程为表头
    if (p_head == NULL) {
        priority_array[p->pri] = p_node;
    } else { // 表头不为空，使用尾插法插入到链表尾部
        while (p_head->next != NULL) {
            p_head = p_head->next;
        }
        p_head->next = p_node;
    }

    // 插入完成，输出当前进程状态
    Log("Successfully create process(id: %d)\n", p->pid);
}

/**
 * 杀死某个进程
 * i：进程x的优先级索引
 * find：进程x找到的标记
 * p：指向进程x
 * p_previous：指向进程x在优先级链表上的前一个节点
 **/
void kill(int pid) {
    int i = 0, find = 0;
    priority_node *p = NULL, *p_previous = NULL;

    // 搜索pid对应的进程
    while (i != MAX_PRIORITY) {
        p = priority_array[i];
        if (p == NULL) {
            i++;
            continue;
        }
        while (p != NULL) {
            if (p->pcb->pid == pid) {
                find = 1;
                break;
            } else {
                p_previous = p;
                p = p->next;
            }
        }
        if (find) {
            break;
        }
        ++i;
    }

    // 未找到进程x，操作终止
    if (!find) {
        Log("Kill: Invaild process number!\n");
        return;
    }

    // 杀死进程，同时摘链
    if (p == priority_array[i]) { // 进程节点在链表头部
        priority_array[i] = priority_array[i]->next;
        id_list[pid] = 0;
        life -= p->pcb->life;
        free(p);
    } else { // 进程节点不在链表头部
        p_previous->next = p->next;
        id_list[pid] = 0;
        life -= p->pcb->life;
        free(p);
    }

    // 操作结束后系统进程状态
    // ps();
}

/**
 * 运行一个时间片周期
 * i：优先级链表索引
 * pri：新的优先级
 * new_pri：新的优先级链表索引
 * prev_r：要运行的进程的上一个节点
 * r：要运行的进程
 **/
int runtime() {
    priority_node *prev_r = NULL, *r = NULL;

    // 寻找优先级最高的，状态为就绪的进程，指针r指向它
    int i = 0;
    for (; i < MAX_PRIORITY; ++i) {
        if (priority_array[i]) {
            r = priority_array[i];
            for (; r && r->pcb->state != READY; prev_r = r, r = r->next);
            if (r) {
                break;
            }
        }
    }

    if (i >= MAX_PRIORITY) {
        Log("No process in the system, create a process and try again!\n");
        return -1;
    }

    // 动态时间片 优先级越高时间片越短，例如第一队列(1~9), 第二队列（10~18)，第三队列(19~27)...
    int quantum = rand() % MAX_PRIORITY + 1 + i * MAX_PRIORITY;
    int old_quantum = quantum;

    // 运行一个时间片周期
    Log("Process(id: %d) gain the CPU time and will execute %d quantum\n", r->pcb->pid, quantum);
    r->pcb->state = RUNNING;    // 将进程状态置为运行中

    Log("Process %d is running...\n", r->pcb->pid);

    // 计算实际执行时间片
    quantum = r->pcb->life < quantum ? r->pcb->life : quantum;
    // 制造运行时间 1时间片=10ms
    usleep(10000 * quantum);
    //    // 制造运行时间
//    for (int j = 0; j < 600000; j++) {
//        for (int k = 0; k < 1000; k++) {
//        }
//    }

    if (r->pcb->life < old_quantum) {
        Log("Past %d quantum before end of process %d life, exit early...\n", r->pcb->life, r->pcb->pid);
    }

    // 更新系统时间
    sys_time += quantum;

    // 更新进程最后一次运行时间
    r->pcb->last_time = sys_time;

    Log("Time out, change to ready status, reduce priority to %d\n", r->pcb->pri + 1);
    // 切换状态为就绪态
    r->pcb->state = READY;
    // 进程每执行一次，优先级降低一级
    r->pcb->pri = r->pcb->pri + 1 > MAX_PRIORITY - 1 ? MAX_PRIORITY - 1 : r->pcb->pri + 1;

    // 更新系统生命周期和进程生命周期
    if (r->pcb->life - quantum > 0) {
        r->pcb->life -= quantum;
        life -= quantum;
    } else {
        life -= r->pcb->life;
        r->pcb->life = 0;
    }

    // 检查进程生命周期是否结束
    if (r->pcb->life == 0) {
        Log("Process %d completed, now release it!\n", r->pcb->pid);
        kill(r->pcb->pid);
    } else {
        // 从当前优先级链表移除
        if (r == priority_array[i]) {
            priority_array[i] = r->next;
        } else {
            prev_r->next = r->next;
        }
        // 添加到新优先级链表的尾部
        int new_pri = r->pcb->pri;
        priority_node *p = priority_array[new_pri];
        if (p) {
            for (; p->next; p = p->next);
            p->next = r;
        } else {
            priority_array[new_pri] = r;
        }
        r->next = NULL;
    }

    // 更新所有进程动态优先级
    update_pri();

    return 0;
    // 一个生命周期结束，显示系统进程状态
    // ps();
}

// 全速运行进程
void run() {
    while (runtime() != -1);
}

// 更新所有进程动态优先级，如果等待大于2倍进程当前寿命，则提升一个优先级（寿命短的更容易被执行）
void update_pri() {
    priority_node *p_node = NULL, *prev_array_node = NULL, *prev_pnode = NULL, *next_pnode = NULL;
    for (int i = 1; i < MAX_PRIORITY; ++i) {
        p_node = priority_array[i];
        while (p_node != NULL) {
            next_pnode = p_node->next;

            // 计算等待时间
            int wait_time = sys_time - p_node->pcb->last_time;

            if (wait_time >= 2 * p_node->pcb->life) {
                Log("Process %d wait time is %d bigger than its double life %d, will be upgrade...\n",
                    p_node->pcb->pid, wait_time, p_node->pcb->life);

                // 从本队列中删除节点
                if (p_node == priority_array[i]) {
                    priority_array[i] = p_node->next;
                } else {
                    prev_pnode->next = p_node->next;
                }

                // 添加到前一条优先级队列
                prev_array_node = priority_array[i - 1];
                if (prev_array_node) {
                    for (; prev_array_node->next; prev_array_node = prev_array_node->next);
                    prev_array_node->next = p_node;
                    // prev_array_node->next->next = NULL;
                } else {
                    priority_array[i - 1] = p_node;
                    // priority_array[i - 1]->next = NULL;
                }
                // 作为新尾部next清空信息
                p_node->next = NULL;

                // 设置上次运行时间为当前系统时间，只升级一次
                p_node->pcb->last_time = sys_time;
                // 优先级提高一级
                --p_node->pcb->pri;

                Log("Process %d upgrade to queue %d from %d\n", p_node->pcb->pid, i - 1, i);
            } else {
                // 如果节点需要提高优先级，prev_node不变
                prev_pnode = p_node;
            }

            p_node = next_pnode;
        }
    }
}
