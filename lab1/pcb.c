//
// Created by Koschei on 2024/2/29.
//

#include <malloc.h>
#include "pcb.h"

// 进程树根节点
pnode *root;
// 进程链表头节点
pnode *plink;

/**
 * 创建进程
 * para[0]: 要创建的新进程的pid
 * para[1]: 新进程的父进程pid
 * para[2]: 新进程的优先级
 **/
int createPCB(pid_t pid, pid_t ppid, int pri) {
    pnode *pp = NULL;

    for (pnode *p = plink; p; p = p->next) {
        if (p->pcb->pid == pid) {
            printf("pid %d is already exist!\n", pid);
            return -1;
        }
        if (p->pcb->pid == ppid) {
            pp = p;
        }
    }

    if (!pp) {
        printf("Parent id %d is not exist!\n", ppid);
        return -2;
    }

    pnode *p = (pnode *) malloc(sizeof(pnode));
    p->pcb = (pcb *) malloc(sizeof(pcb));
    p->pcb->pid = pid;
    p->pcb->ppid = ppid;
    p->pcb->pri = pri;
    p->left = NULL;
    p->right = NULL;
    p->next = NULL;

    if (!pp->left) {
        pp->left = p;
    } else if (!pp->right) {
        pp->right = p;
    } else {
        printf("Parent id %d has two child exist!\n", ppid);
        return -3;
    }

    // 将新进程添加到进程链表中
    pnode *_p;
    for (_p = plink; _p->next; _p = _p->next);
    _p->next = p;
    return 0;
}

// 初始化根节点
void initRoot() {
    root = (pnode *) malloc(sizeof(pnode));
    root->pcb = (pcb *) malloc(sizeof(pcb));
    root->pcb->pid = 0;
    root->pcb->ppid = -1;
    root->pcb->pri = 0;
    root->left = NULL;
    root->right = NULL;
    root->next = NULL;
    plink = root;
}

// 打印进程二叉树
void print_tree(pnode *node, int level) {
    if (node == NULL) return;

    // 递归打印右子树s
    print_tree(node->right, level + 1);

    // 打印当前节点
    for (int i = 0; i < level; ++i) printf("\t");
    printf("PID: %d\n", node->pcb->pid);

    // 递归打印左子树
    print_tree(node->left, level + 1);
}
