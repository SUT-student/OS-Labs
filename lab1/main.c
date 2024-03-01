//
// Created by Koschei on 2024/3/1.
//

#include "pcb.h"

int main() {
    // 所有进程的根进程
    initRoot();

    // 第一层 根进程 -> 2个子进程
    createPCB(1, 0, 1);
    createPCB(2, 0, 2);

    // 第二层
    // 进程1 -> 2个进程
    createPCB(3, 1, 1);
    createPCB(4, 1, 1);

    // 进程2 -> 2个进程
    createPCB(5, 2, 2);
    createPCB(6, 2, 2);

    // 第三层
    // 进程5 -> 1个进程
    createPCB(7, 5, 5);

    // 进程11 -> 2个进程
    createPCB(8, 3, 5);
    createPCB(9, 3, 5);

    // 第四层
    // 进程13 -> 2个进程
    createPCB(10, 7, 6);
    createPCB(11, 7, 6);

    // 进程15 -> 1个进程
    createPCB(12, 9, 7);
    createPCB(13, 9, 7);

    print_tree(root, 0);

    return 0;
}
