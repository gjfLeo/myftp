#include "cmd_client.h"
#include "file.h"
#include "header.h"
#include "msg.h"
#include <stdio.h>

// 登录，成功返回1，失败返回0，出错返回-1
int client_login(int sockfd)
{
    char username[MAX_LENGTH], password[MAX_LENGTH];
    struct ftpmsg msg;

    // 输入用户名和密码
    printf("用户名: ");
    gets(username);
    printf("密码: ");
    gets(password);

    // 发送并检测用户名
    msg.type = LOGIN;
    msg.len = strlen(username) + 1;
    msg.data = username;
    send_msg(sockfd, &msg);  // 发送用户名（LOGIN）
    recv_msg(sockfd, &msg);  // 接收回复（SUCCESS / FAILURE）
    if (msg.type == SUCCESS) // 用户名验证通过
    {
        // 向服务器发送密码
        msg.type = LOGIN;
        msg.len = strlen(password) + 1;
        msg.data = password;
        send_msg(sockfd, &msg);
        recv_msg(sockfd, &msg);
        if (msg.type == SUCCESS) // 密码验证通过
        {
            printf("登录成功，欢迎，%s！\n\n", username);
            return 1;
        }
        else if (msg.type == FAILURE) // 密码错误
        {
            printf("密码错误\n");
            return 0;
        }
        else
        {
            fprintf(stderr, "密码验证时收到意外消息\n");
            return -1;
        }
    }
    else if (msg.type == FAILURE) // 用户名错误
    {
        printf("用户名错误\n");
        return 0;
    }
    else
    {
        fprintf(stderr, "用户名验证时收到意外消息\n");
        return -1;
    }
}
// 本地创建目录
int client_lmkdir(char *line)
{
    if (line == NULL || !strcmp(line, ""))
    {
        fprintf(stderr, "错误：需要参数，正确用法为：lmkdir <本地路径>\n");
        return -1;
    }

    char dir[MAX_LENGTH];
    while (gettoken(dir, line) != -1)
    {
        create_dir(dir);
    }
    return 0;
}
// 本地删除目录
int client_lrmdir(char *line)
{
    if (line == NULL || !strcmp(line, ""))
    {
        fprintf(stderr, "错误：需要参数，正确用法为：lrmdir <本地路径>\n");
        return -1;
    }

    char dir[MAX_LENGTH];
    while (gettoken(dir, line) != -1)
    {
        delete_dir(dir);
    }
    return 0;
}
// 显示本地工作路径
int client_lpwd()
{
    char lpwd[1000];
    if (getcwd(lpwd, 1000) == NULL)
    {
        return -1;
    }
    printf("本地工作路径为：%s\n", lpwd);
    return 0;
}
// 切换本地工作路径
int client_lcd(char *line)
{
    char dir[MAX_LENGTH];
    gettoken(dir, line);

    if (dir == NULL || !strcmp(dir, ""))
    {
        fprintf(stderr, "错误：需要参数，正确用法为：lcd <本地路径>\n");
        return -1;
    }
    if (chdir(dir) == -1)
    {
        fprintf(stderr, "本地路径%s不存在\n", dir);
        return -1;
    }
    return 0;
}
// 显示本地文件
int client_dir(char *line)
{
    char dir[MAX_LENGTH];
    gettoken(dir, line);

    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    // 若参数被省略，则以当前路径作为参数
    if (dir == NULL || !strcmp(dir, ""))
    {
        strcpy(dir, ".");
    }

    if ((dp = opendir(dir)) == NULL)
    {
        fprintf(stderr, "无法打开路径：%s\n", dir);
        return -1;
    }
    printf("本地目录%s下的内容：\n", dir);
    chdir(dir);
    while ((entry = readdir(dp)) != NULL)
    {
        // 忽略.和..
        if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
            continue;
        printf("%s\t", entry->d_name);
    }
    printf("\n");
    closedir(dp);
    return 0;
}

// 服务器创建路径
int client_mkdir(int sockfd, char *line)
{
    if (line == NULL)
    {
        fprintf(stderr, "错误：需要参数，正确用法为：mkdir <服务器路径>\n");
        return -1;
    }
    struct ftpmsg msg;
    char dir[MAX_LENGTH];
    while (gettoken(dir, line) != -1)
    {
        msg.type = C_MKDIR;
        msg.len = strlen(dir) + 1;
        msg.data = dir;
        send_msg(sockfd, &msg);
        recv_msg(sockfd, &msg);
        if (msg.type == SUCCESS)
        {
            // printf("服务器创建目录%s成功\n", dir);
        }
        else if (msg.type == FAILURE)
        {
            fprintf(stderr, "服务器创建目录%s失败\n", dir);
        }
        else
        {
            fprintf(stderr, "mkdir命令收到意外消息\n", dir);
        }
    }
    return 0;
}
// 服务器删除路径
int client_rmdir(int sockfd, char *line)
{
    if (line == NULL)
    {
        fprintf(stderr, "错误：需要参数，正确用法为：rmdir <服务器路径>\n");
        return -1;
    }
    struct ftpmsg msg;
    char dir[MAX_LENGTH];
    while (gettoken(dir, line) != -1)
    {
        msg.type = C_RMDIR;
        msg.len = strlen(dir) + 1;
        msg.data = dir;
        send_msg(sockfd, &msg);
        recv_msg(sockfd, &msg);
        if (msg.type == SUCCESS)
        {
            // printf("服务器删除目录%s成功\n", dir);
        }
        else if (msg.type == FAILURE)
        {
            fprintf(stderr, "服务器删除目录%s失败\n", dir);
        }
        else
        {
            fprintf(stderr, "rmdir命令收到意外消息\n", dir);
        }
    }
    return 0;
}
// 显示服务器工作路径
int client_pwd(int sockfd)
{
    struct ftpmsg msg;
    msg.type = C_PWD;
    msg.len = 0;
    msg.data = NULL;
    send_msg(sockfd, &msg);
    recv_msg(sockfd, &msg);
    if (msg.type == SUCCESS)
    {
        printf("服务器工作路径为：%s\n", msg.data);
        return 0;
    }
    else if (msg.type == FAILURE)
    {
        fprintf(stderr, "获取服务器工作路径失败\n");
        return -1;
    }
}
// 切换服务器工作路径
int client_cd(int sockfd, char *line)
{
    char dir[MAX_LENGTH];
    gettoken(dir, line);

    if (dir == NULL)
    {
        fprintf(stderr, "错误：需要参数，正确用法为：cd <服务器路径>\n");
        return -1;
    }
    struct ftpmsg msg;
    msg.type = C_CD;
    msg.len = strlen(dir) + 1;
    msg.data = dir;
    send_msg(sockfd, &msg);
    recv_msg(sockfd, &msg);
    if (msg.type == SUCCESS)
    {
        // printf("切换服务器工作路径成功\n");
        return 0;
    }
    else if (msg.type == FAILURE)
    {
        fprintf(stderr, "切换服务器工作路径失败\n");
        return -1;
    }
    else
    {
        fprintf(stderr, "cd命令收到意外消息\n");
        return -1;
    }
}
// 显示服务器文件列表
int client_ls(int sockfd, char *line)
{
    char dir[MAX_LENGTH];
    gettoken(dir, line);

    // 若参数被省略，则以当前路径作为参数
    if (dir == NULL || !strcmp(dir, ""))
    {
        strcpy(dir, ".");
    }
    struct ftpmsg msg;
    msg.type = C_LS;
    msg.len = strlen(dir) + 1;
    msg.data = dir;
    send_msg(sockfd, &msg);
    recv_msg(sockfd, &msg);
    if (msg.type == SUCCESS)
    {
        if (msg.data == "")
        {
            printf("服务器目录%s为空\n");
        }
        else
        {
            printf("服务器目录%s下的内容：\n%s\n", dir, msg.data);
        }
        return 0;
    }
    else if (msg.type == FAILURE)
    {
        fprintf(stderr, "获取服务器文件列表失败\n");
        return -1;
    }
    else
    {
        fprintf(stderr, "ls命令收到意外消息\n");
        return -1;
    }
};

// 上传文件
int client_put(int sockfd, char *line)
{
    if (line == NULL)
    {
        fprintf(stderr, "错误：需要参数，正确用法为：put <服务器路径>\n");
        return -1;
    }

    char token[MAX_LENGTH];
    while (gettoken(token, line) != -1)
    {
        send_simple(sockfd, C_PUT);
        send_file(sockfd, token);
    }
    return 0;
}
// 下载文件
int client_get(int sockfd, char *line)
{
    if (line == NULL)
    {
        fprintf(stderr, "错误：需要参数，正确用法为：get <服务器文件路径>\n");
        return -1;
    }
    struct ftpmsg msg;
    char token[MAX_LENGTH];
    while (gettoken(token, line) != -1)
    {
        msg.type = C_GET;
        msg.len = strlen(token) + 1;
        msg.data = token;
        send_msg(sockfd, &msg);
        recv_file(sockfd);
    }
    return 0;
}

int client_help()
{
    printf("\n");
    printf("lmkdir <本地路径>       在本地新建目录\n");
    printf("lrmdir <本地路径>       在本地删除指定的目录（及目录下所有文件）\n");
    printf("lpwd                    显示本地工作路径\n");
    printf("lcd <本地路径>          切换本地工作路径\n");
    printf("dir [<本地路径>]        显示本地指定路径或当前工作路径下的内容\n");
    printf("mkdir <服务器路径>      在服务器上新建目录\n");
    printf("rmdir <服务器路径>      在服务器上删除指定的目录（及目录下所有文件）\n");
    printf("pwd                     显示服务器工作路径\n");
    printf("cd <服务器路径>         切换服务器工作路径\n");
    printf("ls [<服务器路径>]       显示服务器指定路径当前工作路径下的内容\n");
    printf("put <本地文件路径>      向服务器上传指定文件\n");
    printf("get <服务器文件路径>    从服务器下载指定文件\n");
    printf("exit或quit或q           退出程序\n");
    printf("help                    显示此帮助信息\n");
}