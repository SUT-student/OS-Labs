#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

pid_t pid1, pid2;
int wait_mark = 1;

void waiting() {
    while (wait_mark != 0);
}

void stop() {
    wait_mark = 0;
}

int main() {
    // 禁用缓冲区
    // setvbuf(stdout,NULL,_IONBF,0);
    pid1 = fork();

    if (pid1 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        // printf("I am child1 %d", getpid());
        wait_mark = 1;
        // 注册信号处理函数
        // 按下ctrl-c时可能发送信号给所有进程，子进程应该屏蔽此信号
        signal(SIGINT, SIG_IGN);
        signal(SIGTERM, stop);
        waiting();
        printf("Child Process 1 is Killed by Parent!\n");
        exit(0);
    } else {
        // 创建第二个子进程
        pid2 = fork();

        if (pid2 < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid2 == 0) {
            // printf("I am child2 %d\n", getpid());
            wait_mark = 1;
            // 注册信号处理函数
            signal(SIGINT, SIG_IGN);
            signal(SIGTERM, stop);
            waiting();
            printf("Child Process 2 is Killed by Parent!\n");
            exit(0);
        } else {
            // 父进程
            //
            // 为什么不加\n终端不会输出?
            // 对于标准输出，需要输出的数据并不是直接输出到终端上，而是首先缓存到某个地方，当遇到行刷新标志或者该缓存已满的情况下，才会把缓存的数据显示到终端设备上。
            // ANSI C中定义换行符'\n'可以认为是行刷新标志。所以，printf函数没有带'\n'是不会自动刷新输出流，直至缓存被填满。
            //
            // printf("I am parent\n");
            // 注册信号处理函数
            signal(SIGINT, stop);

            waiting();

            kill(pid1, SIGTERM);
            kill(pid2, SIGTERM);

            wait(0);
            wait(0);

            printf("Parent Process is Killed!\n");

            exit(0);
        }
    }
}
