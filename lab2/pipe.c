//
// Created by Koschei on 2024/2/27.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

pid_t pid1, pid2;

int main() {
    int fd[2];
    char in[100], out[100];

    pipe(fd);

    pid2 = fork();

    if (pid2 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid2 == 0) {
        close(fd[0]);
        sprintf(in, "Message from Child 2!\n");
        // 延迟200微秒，子进程1先写入管道
        usleep(200);
        // strlen() 读到'\0'停止，要把'\0'写入应该+1
        write(fd[1], in, strlen(in) + 1); // include '\0'
        close(fd[1]);
        exit(0);
    } else {
        // 创建第二个子进程
        pid1 = fork();

        if (pid1 < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid1 == 0) {
            close(fd[0]);
            sprintf(in, "Message from Child 1!\n");
            write(fd[1], in, strlen(in)); // not include '\0'
            close(fd[1]);
            exit(0);
        } else {
            // 父进程

            wait(0);
            wait(0);

            read(fd[0], out, 50);
            // out[44] = '\0';

            printf("%s", out);

            exit(0);
        }
    }
}
