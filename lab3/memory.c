//
// Created by Koschei on 2024/3/4.
//

#include <stdio.h>
#include <malloc.h>
#include "memory.h"

// 空闲分区链（默认按地址升序排序）
fpnode_t *fplist;
// 使用分区链
upnode_t *uplist;

// 循环首次适应算法
// 记录上次查找结束的位置
fpnode_t *last_fp;

/**
 * 系统初始化
 **/
void init() {
    // 初始化分区表
    ptable_t *ptable = (ptable_t *) malloc(sizeof(ptable_t));
    ptable->start_addr = 0;
    ptable->size = RAM_SIZE;
    ptable->status = false;
    // 初始化空闲分区链
    fplist = (fpnode_t *) malloc(sizeof(fpnode_t));
    fplist->table = ptable;
    fplist->prev = fplist;
    fplist->next = fplist;
    // 初始化使用分区链
    uplist = NULL;
    // 循环首次适应算法
    last_fp = fplist;
}

/**
 * 查看内存使用情况
 **/
void ps() {
    printf("\n=====空闲分区表================\n");
    printf("分区号 | 起始地址 | 大小(MB) | 使用情况\n");
    int i = 1;
    fpnode_t *fpnode = fplist;
    if (fpnode) {
        do {
            printf("%6d %8zu | %8zu | %5d\n", i, fpnode->table->start_addr, fpnode->table->size, fpnode->table->status);
            fpnode = fpnode->next;
            ++i;
        } while (fpnode != fplist);
    }
    printf("===============================\n\n");
    printf("=====已使用的分区表==========\n");
    printf("进程号 | 起始地址 | 大小(MB) | 使用情况\n");
    for (upnode_t *upnode = uplist; upnode; upnode = upnode->next) {
        printf("%6d | %8zu | %8zu | %5d\n", upnode->pid, upnode->table->start_addr, upnode->table->size,
               upnode->table->status);
    }
    printf("=============================\n\n");
}

void allocate(int pid, int alloc_size, bool (*fit_algorithm)(fpnode_t **, int)) {
    if (alloc_size <= 0 || alloc_size > RAM_SIZE) {
        printf("ERROR: 参数错误！\n");
        return;
    }
    if (fplist == NULL) {
        printf("ERROR: 内存已分配完毕，申请失败！\n");
        return;
    }

    fpnode_t *fp = fplist;
    bool find = fit_algorithm(&fp, alloc_size);
    if (!find) {
        printf("ERROR: 内存空间不足，申请失败！\n");
        return;
    }

    // 创建使用分区表
    ptable_t *table = (ptable_t *) malloc(sizeof(ptable_t));
    table->start_addr = fp->table->start_addr;
    table->size = alloc_size;
    table->status = true;

    // 调整空闲分区表
    fp->table->start_addr += alloc_size;
    fp->table->size -= alloc_size;

    // 分区表已经用完
    if (fp->table->size == 0) {
        printf("INFO: 该块内存分配完毕，删除空闲分区表和分区链中对应项\n");
        // 链表中只有一个元素
        if (fp->next == fp) {
            printf("INFO: 已无空闲分区，将空闲链表置为NULL\n");
            fplist = NULL;
            last_fp = NULL;
        } else {
            // fp是链表头
            if (fp == fplist) {
                fplist = fp->next;
            }
            fp->prev->next = fp->next;
            fp->next->prev = fp->prev;
            // 上次查找结果移动至下一个
            last_fp = fp->next;
        }
        free(fp);
    }

    // 创建使用分区链表节点
    upnode_t *self = (upnode_t *) malloc(sizeof(upnode_t));
    self->pid = pid;
    self->table = table;
    self->next = NULL;

    // 插入使用分区链表最后一条，若不存在则成为新头部
    upnode_t *up = uplist;
    if (up) {
        for (; up->next; up = up->next);
        up->next = self;
    } else {
        uplist = self;
    }
}

void release(int start_addr) {
    // 在使用分区链表找到这个节点
    upnode_t *up = uplist, *prev_up;
    if (up) {
        for (; up && up->table->start_addr != start_addr; prev_up = up, up = up->next);
        if (!up) {
            printf("ERROR: 参数错误！\n");
            return;
        }
    } else {
        printf("ERROR: 当前系统未分配内存！\n");
        return;
    }

    // 创建空闲分区表
    ptable_t *table = (ptable_t *) malloc(sizeof(ptable_t));
    table->start_addr = up->table->start_addr;
    table->size = up->table->size;
    table->status = false;

    // 清除使用分区表中该分区
    if (up == uplist) {
        uplist = up->next;
    } else {
        prev_up->next = up->next;
    }
    free(up);

    // 空闲分区链为空，直接创建
    if (fplist == NULL) {
        fplist = (fpnode_t *) malloc(sizeof(fpnode_t));
        fplist->table = table;
        fplist->next = fplist;
        fplist->prev = fplist;
        last_fp = fplist;
        return;
    }

    // 非空链情况，寻求与前后空闲分区合并
    fpnode_t *fp = fplist;

    // 创建新空闲分区链节点
    fpnode_t *self = (fpnode_t *) malloc(sizeof(fpnode_t));;
    self->table = table;

    // 空闲分区链仅存在一个元素 不可能存在左右都相邻
    if (fp->next == fp) {
        // printf("空闲分区链仅存在一个元素\n"); debug
        // 左相邻 加size 不插入
        if (fp->table->start_addr + fp->table->size == table->start_addr) {
            fp->table->size += table->size;
        }
            // 右相邻 加size 修改右边的起始addr和size 不插入
        else if (table->start_addr + table->size == fp->table->start_addr) {
            fp->table->start_addr = table->start_addr;
            fp->table->size += table->size;
        }
            // 都不相邻 按起始地址升序放入
        else if (table->start_addr < fp->table->start_addr) {
            // 放前面
            self->prev = fp;
            self->next = fp;
            fp->prev = self;
            fp->next = self;
            // 成为新头部
            fplist = self;
        } else {
            // 放后面
            self->prev = fp;
            self->next = fp;
            fp->prev = self;
            fp->next = self;
        }
        return;
    }

    // 至少存在两个不相邻的空闲分区，找到空闲分区链中第一个比分区表地址大于或等于的位置（地址升序）
    bool find = false;
    do {
        if (fp->table->start_addr >= table->start_addr + table->size) {
            find = true;
            break;
        }
        fp = fp->next;
    } while (fp != fplist);

    // 没有找到 所有的起始地址都比分区表起始地址小，直接插入最后，只需判断是否左相邻
    if (!find) {
        // debug
        // printf("没有找到 所有的起始地址都比分区表起始地址小\n");
        fpnode_t *tail = fplist->prev;
        // 左相邻 加size 不插入
        if (tail->table->start_addr + tail->table->size == table->start_addr) {
            tail->table->size += table->size;
        }
            // 否则插入尾部
        else {
            self->prev = tail;
            self->next = fplist;
            tail->next = self;
        }
        return;
    }

    // 找到，则fp为右空闲分区，前一个为左空闲分区
    // debug
    // printf("找到，则fp为右空闲分区，前一个为左空闲分区");
    fpnode_t *left_fp = fp->prev;
    bool left = left_fp->table->start_addr + left_fp->table->size == table->start_addr;
    bool right = table->start_addr + table->size == fp->table->start_addr;
    // 左右都相邻 左边加size，删除右边fp
    if (left && right) {
        left_fp->table->size += table->size + fp->table->size;
        left_fp->next = fp->next;
        fp->next->prev = left_fp;
    }
        // 左相邻 加size 不插入
    else if (left) {
        left_fp->table->size += table->size;
    }
        // 右相邻 加size 修改右边的起始addr和size
    else if (right) {
        fp->table->start_addr = table->start_addr;
        fp->table->size += table->size;
    }
        // 都不相邻 直接插入
    else {
        self->prev = left_fp;
        self->next = fp;
        left_fp->next = self;
        fp->prev = self;
        // 如果插入位置为头部 成为新头部
        if (fp == fplist) {
            fplist = self;
        }
    }
}

// 首次适应算法
bool first_fit(fpnode_t **fp, int alloc_size) {
    // 循环指针
    fpnode_t *p = fplist;
    do {
        if (p->table->size >= ALLOC_MIN_SIZE + alloc_size) {
            *fp = p;
            return true;
        }
        p = p->next;
    } while (p != fplist);
    return false;
}

// 循环首次适应算法
bool next_fit(fpnode_t **fp, int alloc_size) {
    // 循环指针
    fpnode_t *p = last_fp;
    do {
        if (p->table->size >= ALLOC_MIN_SIZE + alloc_size) {
            *fp = p;
            // 记录找到的位置
            last_fp = p;
            return true;
        }
        p = p->next;
    } while (p != last_fp);
    return false;
}

// 最佳适应算法
bool best_fit(fpnode_t **fp, int alloc_size) {
    bool find = false;
    // 循环指针
    fpnode_t *p = fplist, *best_p = NULL;
    // 查找满足alloc_size的最小空闲容量
    do {
        if ((best_p == NULL || best_p->table->size > p->table->size) && p->table->size >= ALLOC_MIN_SIZE + alloc_size) {
            find = true;
            best_p = p;
        }
        p = p->next;
    } while (p != fplist);
    *fp = best_p;
    return find;
}
