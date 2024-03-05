//
// Created by Koschei on 2024/3/2.
//

#include <stddef.h>
#include <stdio.h>
#include "page.h"

page_t page[total_pages];

pfc_t pfc[MAX_PAGE_FRAMES]; // 页框控制块
pfc_t *free_pf_head;    // 空页框头指针
pfc_t *busy_pf_head;    // 忙页框头指针
pfc_t *busy_pf_tail;    // 忙页框尾指针

int invalid_pages;              // 页面失效次数
int op[total_instructions];     // 指令流数组
int op2page[total_instructions];   // 每条指令所属页号
int op2offset[total_instructions]; // 页号偏移量

/* 初始化数据结构
 * page_frames: 内存页框数
 */
void init(int page_frames) {
    invalid_pages = 0;

    // 初始化进程页面
    for (int i = 0; i < total_pages; ++i) {
        page[i].page_no = i;
        page[i].page_frame_no = INVALID;
        page[i].time = 0;
    }

    // 用户可用内存容量8K，进程页面1K，内存有8个页框
    // 初始化内存页框控制块
    for (int i = 0; i < page_frames; ++i) {
        pfc[i].page_no = INVALID;
        pfc[i].page_frame_no = i;
        pfc[i].next = &pfc[i + 1];
    }
    pfc[page_frames - 1].page_frame_no = page_frames - 1;
    pfc[page_frames - 1].next = NULL;

    // 初始化空页框队列头指针
    free_pf_head = &pfc[0];

    // 初始化忙页框队列头指针
    busy_pf_head = busy_pf_tail = NULL;

    // 程序前8页调入内存
    for (int i = 0; i < 8; ++i) {
        pfc[i].page_no = i;
        page[i].page_frame_no = i;
    }
}

/* 先进先出置换算法
 * page_frames: 内存页框数
 */
void FIFO(int page_frames) {
    init(page_frames);
    for (int i = 0; i < total_instructions; ++i) {
        int page_id = op2page[i];
        // 页面失效 不在内存中
        if (page[page_id].page_frame_no == INVALID) {
            ++invalid_pages; // 缺页加1

            // 无空闲页框
            if (free_pf_head == NULL) {
                // 忙页框队头指针
                pfc_t *free_fp = busy_pf_head;
                // 忙页框队头指向下一个
                busy_pf_head = busy_pf_head->next;
                // 取消进程页面与忙页框队头的关联
                page[free_fp->page_no].page_frame_no = INVALID;
                // 取忙页框队列队头作为第一个空闲页框
                free_pf_head = free_fp;
                free_pf_head->next = NULL;
            }

            // FIFO 调入新页面到内存页框
            pfc_t *busy_pf = free_pf_head;
            // 空闲页框队列指向下一个
            free_pf_head = free_pf_head->next;

            // 空闲页框与进程页面号关联
            busy_pf->page_no = page_id;
            page[page_id].page_frame_no = busy_pf->page_frame_no;

            // 插入忙页框队列
            if (busy_pf_tail == NULL) {
                busy_pf_head = busy_pf_tail = busy_pf;
            } else {
                busy_pf_tail->next = busy_pf;
                busy_pf_tail = busy_pf;
            }

            // 忙队列尾部next置空
            busy_pf_tail->next = NULL;
        }
    }
    printf("FIFO:%6.4f ", invalid_pages / 320.0);
}

/* 最近最久未使用置换算法
 * page_frames: 内存页框数
 */
void LRU(int page_frames) {
    int cur_time = 0;
    int min_time = INVALID, replaced_page_frame = INVALID;
    init(page_frames);

    for (int i = 0; i < total_instructions; ++i) {
        int page_id = op2page[i];
        // 页面失效 不在内存中
        if (page[page_id].page_frame_no == INVALID) {
            ++invalid_pages; // 缺页加1

            // 无空闲页框
            if (free_pf_head == NULL) {
                min_time = INVALID, replaced_page_frame = INVALID;
                // 找到最早被使用的页框（time最小）置换出去
                for (int j = 0; j < page_frames; ++j) {
                    int page_no = pfc[j].page_no;
                    if (page_no != INVALID && (min_time == INVALID || page[page_no].time < min_time)) {
                        min_time = page[page_no].time;
                        replaced_page_frame = j;
                    }
                }

                // 作为空闲页框队头
                free_pf_head = &pfc[replaced_page_frame];
                // 取消进程页面与内存页框的关联
                page[free_pf_head->page_no].page_frame_no = INVALID;
                free_pf_head->page_no = INVALID;
                free_pf_head->next = NULL;
            }

            // LRU 调入新页面到内存页框
            pfc_t *busy_pf = free_pf_head;
            // 空闲页框队列指向下一个
            free_pf_head = free_pf_head->next;

            // 空闲页框与进程页面号关联
            busy_pf->page_no = page_id;
            page[page_id].page_frame_no = busy_pf->page_frame_no;
        }

        // 记录页面被使用时间
        page[page_id].time = cur_time;
        // 时间增加一个指令周期
        ++cur_time;
    }
    printf("LRU:%6.4f ", invalid_pages / 320.0);
}

/* 最佳置换置换算法
 * page_frames: 内存页框数
 */
void OPT(int page_frames) {
    // 记录每个页面离起点的距离，置换距离最大的页面（最长时间内不会被访问）
    int dist[total_pages], max_dist;
    int max_dist_page_frame;
    init(page_frames);

    for (int i = 0; i < total_instructions; ++i) {
        int page_id = op2page[i];
        // 页面失效 不在内存中
        if (page[page_id].page_frame_no == INVALID) {
            ++invalid_pages; // 缺页加1

            // 无空闲页框
            if (free_pf_head == NULL) {
                // 设置在内存中的页面的距离为最大值，如果某页面以后不会再被访问，则距离为最大被选中
                for (int j = 0; j < total_pages; ++j) {
                    if (page[j].page_frame_no != INVALID) {
                        dist[j] = total_instructions;
                    }
                }

                // 更新i+1后面的指令所属页面距当前的距离
                for (int k = i + 1, d = 1; k < total_instructions; ++d, ++k) {
                    // 仅判断在内存中的页面
                    if (page[op2page[k]].page_frame_no != INVALID) {
                        dist[op2page[k]] = d;
                    }
                }

                // 找到距离最大的页面
                max_dist = -1;
                for (int j = 0; j < page_frames; ++j) {
                    if (dist[pfc[j].page_no] > max_dist) {
                        max_dist = dist[pfc[j].page_no];
                        max_dist_page_frame = j;
                    }
                }

                // 作为空闲页框队头
                free_pf_head = &pfc[max_dist_page_frame];
                // 取消进程页面与内存页框的关联
                page[free_pf_head->page_no].page_frame_no = INVALID;
                free_pf_head->page_no = INVALID;
                free_pf_head->next = NULL;
            }

            // OPT 调入新页面到内存页框
            pfc_t *busy_pf = free_pf_head;
            // 空闲页框队列指向下一个
            free_pf_head = free_pf_head->next;

            // 空闲页框与进程页面号关联
            busy_pf->page_no = page_id;
            page[page_id].page_frame_no = busy_pf->page_frame_no;
        }
    }
    printf("OPT:%6.4f ", invalid_pages / 320.0);
}
