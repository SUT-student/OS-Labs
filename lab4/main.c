//
// Created by Koschei on 2024/3/2.
//

#include <stdio.h>
#include "dispatcher.h"

/**
 * 命令行控制台
 **/
void terminal() {
    char cmdstr[32];
    int cmd = 0, id = 0;
    int cnt;
    while (1) {
        printf("cmd: ");
        scanf("%s", cmdstr);

        for (int i = 0; i < 9; i++) {
            if (strcmp(command[i], cmdstr) == 0) {
                cmd = i;
                break;
            }
        }

        switch (cmd) {
            case 0:
                return;
            case 1:
                printf("\n=================================================\n");
                printf("id: 当前进程ID  status: 进程状态:0-就绪,1-运行  pri: 进程优先级  life: 进程生命\n");
                printf("随机创建一个新进程: create\n");
                printf("杀死一个进程: kill\n");
                // printf("使一个进程进入阻塞状态: sleep\n");
                // printf("唤醒一个进程: awake\n");
                printf("查看当前进程信息: ps\n");
                printf("运行一个时间片周期: runtime\n");
                printf("全速运行: run\n");
                printf("获取帮助: help\n");
                printf("退出: exit\n");
                printf("=================================================\n\n");
                break;
            case 2:
                ps();
                break;
            case 3:
                scanf("%d", &cnt);
                for (int i = 0; i < cnt; ++i) {
                    create();
                }
                break;
            case 4:
                printf("Which process you want to kill: ");
                scanf("%d", &id);
                kill(id);
                break;
            case 5:
                runtime();
                break;
            case 6:
                // unuse
//                printf("Which process you want to sleep: ");
//                scanf("%d", &id);
                // sleep(id);
                break;
            case 7:
                // unuse
//                printf("Which process you want to awake: ");
//                scanf("%d", &id);
                // awake(id);
                break;
            case 8:
                run();
                break;
            default:
                printf("Error command!\n");
        }
    }
}

int main() {
    init();
    terminal();
    return 0;
}
