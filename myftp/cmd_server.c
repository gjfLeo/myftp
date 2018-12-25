#include "cmd_server.h"
#include "file.h"
#include "header.h"
#include "msg.h"
#include <stdio.h>

// 定义测试时的用户名和密码
#define USERNAME "admin"
#define PASSWORD "12345"

/* 接收到LOGIN
判断用户名
    用户名正确：发送SUCCESS，等待回复
        接收LOGIN，判断密码
            密码正确：发送SUCCESS，等待下条消息
            密码错误：发送FAILURE，等待下条消息
    用户名错误：发送FAILURE，等待下条消息       */
int c_login(int sockfd, char *username)
{
    struct ftpmsg msg;
    // 登录信息
    if (!strcmp(username, USERNAME))
    {
        // 用户名正确
        send_simple(sockfd, SUCCESS); // 回复SUCCESS
        recv_msg(sockfd, &msg);       // 接收密码（LOGIN）
        // 验证密码
        if (msg.type != LOGIN)
        {
            fprintf(stderr, "密码验证时收到意外消息\n");
            return -1;
        }
        if (!strcmp(msg.data, PASSWORD))
        {
            // 密码正确
            printf("客户端%s登录成功\n", username);
            send_simple(sockfd, SUCCESS); // 回复SUCCESS
            return 0;
        }
        else
        {
            // 密码错误
            printf("客户端%s登录信息错误\n", username);
            send_simple(sockfd, FAILURE); // 回复FAILURE
            return 0;
        }
    }
    else
    {
        // 用户名错误
        printf("客户端%s登录信息错误\n", username);
        send_simple(sockfd, FAILURE); // 回复FAILURE
        return 0;
    }
}

int c_mkdir(int sockfd, char *dir)
{
    if (create_dir(dir) == -1)
    {
        fprintf(stderr, "创建目录%s失败\n", dir);
        send_simple(sockfd, FAILURE);
        return -1;
    }
    else
    {
        printf("创建目录%s成功\n", dir);
        send_simple(sockfd, SUCCESS);
        return 0;
    }
}

int c_rmdir(int sockfd, char *dir)
{
    if (delete_dir(dir) == -1)
    {
        fprintf(stderr, "删除目录%s失败\n", dir);
        send_simple(sockfd, FAILURE);
        return -1;
    }
    else
    {
        printf("删除目录%s成功\n", dir);
        send_simple(sockfd, SUCCESS);
        return 0;
    }
}

int c_pwd(int sockfd)
{
    char pwd[MAX_LENGTH];
    if (getcwd(pwd, MAX_LENGTH) == NULL)
    {
        fprintf(stderr, "getcwd函数出错，获取工作路径失败\n");
        send_simple(sockfd, FAILURE);
        return -1;
    }
    else
    {
        struct ftpmsg msg;
        msg.type = SUCCESS;
        msg.len = strlen(pwd) + 1;
        msg.data = pwd;
        printf("显示工作路径成功\n");
        send_msg(sockfd, &msg);
        return 0;
    }
}

int c_cd(int sockfd, char *dir)
{
    if (chdir(dir) == -1)
    {
        fprintf(stderr, "切换工作路径失败\n");
        send_simple(sockfd, FAILURE);
        return -1;
    }
    else
    {
        printf("切换工作路径成功\n");
        send_simple(sockfd, SUCCESS);
        return 0;
    }
}

int c_ls(int sockfd, char *dir)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    char result[MAX_LENGTH] = "";

    if ((dp = opendir(dir)) == NULL)
    {
        fprintf(stderr, "显示文件列表失败，无法打开路径：%s\n", dir);
        send_simple(sockfd, FAILURE);
        return -1;
    }
    chdir(dir);
    while ((entry = readdir(dp)) != NULL)
    {
        // 忽略.和..
        if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
            continue;
        strcat(result, entry->d_name);
        // printf("result += %s\n", entry->d_name);
        strcat(result, "\t");
    }
    struct ftpmsg msg;
    msg.type = SUCCESS;
    msg.len = strlen(result) + 1;
    msg.data = result;
    send_msg(sockfd, &msg);
    printf("显示文件列表成功\n");
    // printf("%s\n", result);
    closedir(dp);
    return 0;
}

int c_put(int sockfd)
{
    return recv_file(sockfd);
}

int c_get(int sockfd, char *path)
{
    return send_file(sockfd, path);
}