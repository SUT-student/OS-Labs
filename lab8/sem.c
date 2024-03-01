//
// Created by Koschei on 2024/3/1.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sem.h"

# define SEMAPHORE_NUMBER 5
# define PROCESS_NUMBER 20

// 定义5个信号量
semaphore *sem[SEMAPHORE_NUMBER];
// 定义0-19一共20个进程
pcb *pr[PROCESS_NUMBER];

void init() {
    for (int i = 0; i < SEMAPHORE_NUMBER; ++i) {
        sem[i] = (semaphore *) malloc(sizeof(semaphore));
        sem[i]->count = 1;
        sem[i]->cur_pid = -1;
        sem[i]->wait_list = NULL;
        sprintf(sem[i]->name, "S%d", i);
    }

    for (int i = 0; i < PROCESS_NUMBER; ++i) {
        pcb *p = (pcb *) malloc(sizeof(pcb));
        p->pid = i;
        p->ppid = -1;
        p->pri = 1;

        pr[i] = p;
    }
}

// 申请资源 -> P操作
void p(char *name, pid_t pid) {
    pcb *p = NULL;
    semaphore *s = NULL;

    for (int i = 0; i < SEMAPHORE_NUMBER; ++i) {
        if (strcmp(sem[i]->name, name) == 0) {
            s = sem[i];
            break;
        }
    }

    if (!s) {
        printf("Semaphore %s not exist.\n", name);
        return;
    }

    for (int i = 0; i < PROCESS_NUMBER; ++i) {
        if (pr[i]->pid == pid) {
            p = pr[i];
            break;
        }
    }

    if (!p) {
        printf("Process %d not exist.\n", pid);
        return;
    }

    // 信号量-1
    --s->count;
    if (s->count >= 0) {
        s->cur_pid = pid;
    } else {
        pnode *p_node = (pnode *) malloc(sizeof(pnode));
        p_node->pcb = p;
        p_node->next = NULL;
        p_node->left = NULL;
        p_node->right = NULL;

        if (s->wait_list) {
            pnode *_p = s->wait_list;
            for (; _p->next; _p = _p->next);
            _p->next = p_node;
        } else {
            s->wait_list = p_node;
        }
    }
}

// 释放资源 -> V操作
void v(char *name) {
    semaphore *s = NULL;

    // 查找信号量
    for (int i = 0; i < SEMAPHORE_NUMBER; ++i) {
        if (strcmp(sem[i]->name, name) == 0) {
            s = sem[i];
            break;
        }
    }

    if (!s) {
        printf("Semaphore %s not exist.\n", name);
        return;
    }

    /**
	 * 释放资源
	 * 如果等待列表中有进程，count数量不变，等待列表队头入队
	 * 如果等待列表中无进程，count数量++
	 **/
    if (s->wait_list) {
        s->cur_pid = s->wait_list->pcb->pid;
        s->wait_list = s->wait_list->next;
        return;
    }
    ++s->count;
}

// 查看临界资源使用状态
void print_resources() {
    printf("\n===================================================\n");
    for (int i = 0; i < SEMAPHORE_NUMBER; i++) {
        if (sem[i]->count <= 0) {
            printf("%s (Current process id: %d) | Wait list: ", sem[i]->name, sem[i]->cur_pid);
            pnode *p = sem[i]->wait_list;
            while (p != NULL) {
                printf("%5d ->", p->pcb->pid);
                p = p->next;
            }
            printf("   List end\n");
        } else {
            printf("%s now avaliable\n", sem[i]->name);
        }
    }
    printf("===================================================\n\n");
}
