//
// Created by Koschei on 2024/2/28.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MSG_SIZE 1024

int msqid;
key_t key;

// 消息结构体
struct message {
    long mtype;           // 消息类型
    char mtext[MSG_SIZE]; // 消息内容
};

struct message snd_msg;
struct message rcv_msg;

void server() {
    // 创建消息队列
    if ((msqid = msgget(key, IPC_CREAT | 0666)) == -1) {
        perror("msgget");
        exit(1);
    }

    do {
        // 阻塞接受队列中的第一条消息
        msgrcv(msqid, &rcv_msg, MSG_SIZE, 0, 0);
        printf("[server] received! (msg: %s)\n", rcv_msg.mtext);
    } while (rcv_msg.mtype != 1);

    // 删除消息队列
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    exit(0);
}

void client() {
    // 得到消息队列id
    msqid = msgget(key, 0666);

    for (int i = 10; i >= 1; --i) {
        snd_msg.mtype = i;
        sprintf(snd_msg.mtext, "Hello, I am message %ld", snd_msg.mtype);
        msgsnd(msqid, &snd_msg, MSG_SIZE, 0);
        printf("[client] send! (msg: %s)\n", snd_msg.mtext);
    }

    exit(0);
}

int main() {
    // 使用当前目录和'a'生成key
    key = ftok(".", 'a');

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        server();
    } else {
        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            client();
        } else {
            wait(0);
            wait(0);

            exit(0);
        }
    }
}
