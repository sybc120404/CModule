#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVER_PORT 18888 //端口号不能发生冲突,不常用的端口号通常大于 5000

int main()
{
    struct sockaddr_in server_addr = {};
    struct sockaddr_in client_addr = {};
    char ip_str[20] = {};
    int sockfd = 0;
    int connfd = 0;
    int addrlen = sizeof(client_addr);
    char recvbuf[512] = {};
    int ret = 0;

    /* 打开套接字，得到套接字描述符 */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    /* 将套接字与指定端口号绑定 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    ret = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(ret < 0)
    {
        perror("bind error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* server进入监听模式 */
    ret = listen(sockfd, 50);
    if(ret < 0)
    {
        perror("listen error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* 阻塞等待客户端连接 */
    connfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
    if(connfd < 0)
    {
        perror("accept error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("client login\r\n");
    inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip_str, sizeof(ip_str));
    printf("client ip addr: %s\r\n", ip_str);
    printf("client port: %d\r\n", client_addr.sin_port);

    /* 接受客户端发送数据  */
    while(1)
    {
        memset(recvbuf, 0, sizeof(recvbuf));

        /* 读取数据 */
        ret = recv(connfd, recvbuf, sizeof(recvbuf), 0);
        if(ret < 0)
        {
            perror("recv error");
            close(connfd);
            break;
        }

        /*打印 */
        printf("from client: %s\r\n", recvbuf);

        if(!strncmp(recvbuf, "exit", 4))
        {
            printf("server exit.\r\n");
            close(connfd);
            break;
        }
    }

    close(sockfd);
    exit(EXIT_SUCCESS);

    return 0;
}