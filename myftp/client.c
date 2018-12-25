#include "cmd_client.h"
#include "header.h"
#include "msg.h"

int main(int argc, char const *argv[])
{
    char ip[20] = DEFAULT_IP;
    int port = DEFAULT_PORT;
    if (argc == 2) // 只有一个参数时
    {
        port = atoi(argv[1]); // 将参数作为端口号
    }
    else if (argc > 2) // 有两个（或更多）参数时
    {
        strcpy(ip, argv[1]);  // 将第一个参数作为IP地址
        port = atoi(argv[2]); // 将第二个参数作为端口号
    }

    int sockfd, len, result;
    struct sockaddr_in address;

    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    /*  AF_INET:        ARPA因特网协议（UNIX网络套接字）
        SOCK_STREAM:    流套接字
        0:              使用默认协议	                */
    if (sockfd == -1)
    {
        fprintf(stderr, "socket函数出错，客户端创建套接字失败\n");
        return -1;
    }

    // 套接字地址
    address.sin_family = AF_INET;            // 域
    address.sin_addr.s_addr = inet_addr(ip); // IP地址
    address.sin_port = htons(port);          // 端口号

    // 计算长度
    len = sizeof(address);

    // 请求连接
    result = connect(sockfd, (struct sockaddr *)&address, len);
    if (result == -1)
    {
        fprintf(stderr, "connect函数出错，客户端请求连接失败，请检查服务器是否正在运行\n");
        return -1;
    }

    struct ftpmsg msg;

    // 开启命令行

    // 用户登录，直到成功
    printf("连接服务器成功，请登录：\n");
    while (client_login(sockfd) != 1)
    {
    }

    // 登录成功后的循环，命令执行完则continue，quit或exit命令则break
    char line[MAX_LENGTH];
    char cmd[MAX_LENGTH];
    while (1)
    {
        printf("myftp > ");
        gets(line);
        gettoken(cmd, line);
        if (cmd == NULL) // 指令为空
        {
            continue;
        }
        else if (!strcmp(cmd, "lmkdir"))
        {
            client_lmkdir(line);
        }
        else if (!strcmp(cmd, "lrmdir"))
        {
            client_lrmdir(line);
        }
        else if (!strcmp(cmd, "lpwd"))
        {
            client_lpwd(line);
        }
        else if (!strcmp(cmd, "lcd"))
        {
            client_lcd(line);
        }
        else if (!strcmp(cmd, "dir"))
        {
            client_dir(line);
        }
        else if (!strcmp(cmd, "mkdir"))
        {
            client_mkdir(sockfd, line);
        }
        else if (!strcmp(cmd, "rmdir"))
        {
            client_rmdir(sockfd, line);
        }
        else if (!strcmp(cmd, "pwd"))
        {
            client_pwd(sockfd);
        }
        else if (!strcmp(cmd, "cd"))
        {
            client_cd(sockfd, line);
        }
        else if (!strcmp(cmd, "ls"))
        {
            client_ls(sockfd, line);
        }
        else if (!strcmp(cmd, "put"))
        {
            client_put(sockfd, line);
        }
        else if (!strcmp(cmd, "get"))
        {
            client_get(sockfd, line);
        }
        else if (!strcmp(cmd, "exit") || !strcmp(cmd, "quit") || !strcmp(cmd, "q"))
        {
            send_simple(sockfd, C_QUIT);
            break;
        }
        else if (!strcmp(cmd, "help"))
        {
            client_help();
        }
        else
        {
            printf("错误的指令，使用help命令查看指令列表\n");
        }
    }

    // 关闭套接字
    close(sockfd);

    return 0;
}