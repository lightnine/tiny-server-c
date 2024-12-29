#include "csapp.h"

void echo(int connfd);

/*
 * main - Main routine for the echo server
 * 这里只有一个进程进行处理，每次只能处理一个连接。
 * 前一个连接如果未处理完，则后一个连接是无法建立连接的，只能等待
 */
int main(int argc, char** argv) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    // listenfd 是监听套接字描述符，connfd 是连接套接字描述符
    listenfd = Open_listenfd(argv[1]);
    printf("server listen on port %s\n", argv[1]);
    while (1) {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
        Getnameinfo((SA*)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
        // 这里如果客户端不关闭，那么这里echo会一直阻塞读取connfd中的数据
        // 客户端关闭后，会读取到EOF，然后关闭connfd，然后进入下一次的循环
        echo(connfd);
        Close(connfd);
        printf("Connection closed, connfd: %d\n", connfd);
    }
    exit(0);
}