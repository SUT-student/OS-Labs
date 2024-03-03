//
// Created by Koschei on 2024/3/3.
//

#include <stdio.h>
#include <string.h>
#include "bank.h"

void terminal() {
    char cmdstr[32];
    int cmd, req_pid, req_list[RESOURCES];
    while (1) {
        printf("cmd: ");
        scanf("%s", cmdstr);

        if (strcmp(cmdstr, "exit") == 0) {
            return;
        }

        if (strcmp(cmdstr, "help") == 0) {
            printf("\n=================================================\n");
            printf("请求资源: request\n");
            printf("查看当前银行家账本: ps\n");
            printf("获取帮助: help\n");
            printf("退出: exit\n");
            printf("=================================================\n\n");
            continue;
        }

        if (strcmp(cmdstr, "ps") == 0) {
            ps();
            continue;
        }

        if (strcmp(cmdstr, "request") == 0) {
            printf("要申请资源的进程ID: ");
            scanf("%d", &req_pid);
            printf("要申请的资源的数量: ");
            for (int i = 0; i < RESOURCES; ++i) {
                scanf("%d", &req_list[i]);
            }
            // scanf("%d %d %d", &req_list[0], &req_list[1], &req_list[2]);
            request(req_pid, req_list);
            continue;
        }

        printf("cmd: 未知的命令！\n");
    }
}

int main() {
    init();
    ps();
    terminal();
    return 0;
}
