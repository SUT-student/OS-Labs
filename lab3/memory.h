//
// Created by Koschei on 2024/3/4.
//

#ifndef OS_LABS_MEMORY_H
#define OS_LABS_MEMORY_H

#include <stdbool.h>
#include <stddef.h>

// 内存容量，单位为MB
#define RAM_SIZE 64
// 不可再分割的内存大小
#define ALLOC_MIN_SIZE 0

typedef struct FPCHAIN fpnode_t;
typedef struct UPCHAIN upnode_t;
typedef struct PTABLE ptable_t;

/**
 * 分区表(Partition table)
 * 起始地址(内存地址-kb) | 大小(kb) | 使用情况(false:未使用 true:使用中)
 * start_addr | size | status
 **/
struct PTABLE {
    size_t start_addr;
    size_t size;
    bool status;
};

/**
 * 空闲分区链(Free Partition Chain)
 * table: 指向对应的空闲分区表地址
 * previous: 双向链表的上一项
 * next: 双向链表的下一项
 **/
struct FPCHAIN {
    ptable_t *table;
    fpnode_t *prev;
    fpnode_t *next;
};

/**
 * 使用中分区链(Used Partition Chain)
 * pid: 使用该分区的进程号
 * table: 指向对应的空闲分区表地址
 * next: 单向链表的下一项
 **/
struct UPCHAIN {
    int pid;
    ptable_t *table;
    upnode_t *next;
};

void init();

void ps();

void allocate(int, int);

void release(int);

// 首次适应算法
bool first_fit(fpnode_t **, int);

// 循环首次适应算法
bool next_fit(fpnode_t **, int);

// 最佳适应算法
void best_fit();

#endif //OS_LABS_MEMORY_H
