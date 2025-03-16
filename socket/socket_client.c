#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVER_PORT     18888 //服务器的端口号
#define SERVER_IP       "192.168.1.102" //服务器的 IP 地址
int main(void)
{
    struct sockaddr_in server_addr = {};
    char buf[512] = {};
    int sockfd = 0;
    int ret = 0;

    /* 打开套接字，得到套接字描述符 */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > sockfd) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    /* 调用 connect 连接远端服务器 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT); //端口号
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);//IP 地址
    ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (0 > ret) 
    {
        perror("connect error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("connet server ok\r\n");

    /* 向服务器发送数据 */
    while(1)
    {
        // 清理缓冲区
        memset(buf, 0x0, sizeof(buf));

        // 接收用户输入的字符串数据
        printf("Please enter a string: ");
        fgets(buf, sizeof(buf), stdin);

        // 将用户输入的数据发送给服务器
        ret = send(sockfd, buf, strlen(buf), 0);
        if(0 > ret)
        {
            perror("send error");
            break;
        }
        //输入了"exit"，退出循环
        if(!strncmp(buf, "exit", 4))
            break;
    }

    close(sockfd);
    exit(EXIT_SUCCESS);
}