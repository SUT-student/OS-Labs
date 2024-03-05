//
// Created by Koschei on 2024/3/2.
//

#ifndef OS_LABS_PAGE_H
#define OS_LABS_PAGE_H

#define INVALID (-1)
#define total_instructions 320
#define instructions_per_page 10
#define total_pages (total_instructions / instructions_per_page) // virtual pages
#define MAX_PAGE_FRAMES 32

// 页面类型（进程逻辑地址空间）
typedef struct {
    int page_no;       // 页号 page_number
    int page_frame_no; // 页框号 page_frame_number
    int time;          // 上次被访问时间 LRU
} page_t;

extern page_t page[total_pages];

// 内存页框控制结构(page_frame_control)
typedef struct pfc_struct pfc_t;

// 页面控制结构
struct pfc_struct {
    int page_no;
    int page_frame_no;
    pfc_t *next;
};

extern pfc_t pfc[MAX_PAGE_FRAMES];       // 页框控制块
extern pfc_t *free_pf_head; // 空页框头指针
extern pfc_t *busy_pf_head; // 忙页框头指针
extern pfc_t *busy_pf_tail; // 忙页框尾指针

extern int invalid_pages;              // 页面失效次数
extern int op[total_instructions];     // 指令流数组
extern int op2page[total_instructions];   // 每条指令所属页号
extern int op2offset[total_instructions]; // 页内偏移量

void init(int);

void OPT(int);

void FIFO(int);

void LRU(int);

#endif //OS_LABS_PAGE_H
