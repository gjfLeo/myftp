#include "msg.h"
#include "file.h"
#include "header.h"

int send_msg(int sockfd, struct ftpmsg *msg)
{
    int result;

    // 发送type
    result = send(sockfd, &msg->type, sizeof(int), 0);
    if (result == -1)
    {
        fprintf(stderr, "send函数出错，type发送失败\n");
        return -1;
    }
    // printf("> 发送信息：type = %d", msg->type);

    // 发送len
    result = send(sockfd, &msg->len, sizeof(int), 0);
    if (result == -1)
    {
        fprintf(stderr, "send函数出错，len发送失败\n");
        return -1;
    }
    // printf(", len = %d", msg->len);

    // 发送data
    // printf(", data = %-30.30s\n", msg->data);
    if (msg->len == 0)
    {
        return 0;
    }
    int sent = 0;           // 总共已发送的位数
    while (sent < msg->len) // 循环直到data发送完毕
    {
        result = send(sockfd, msg->data + sent, msg->len - sent, 0);
        // result返回值为-1表示发送失败，否则为本次发送成功的位数
        if (result == -1)
        {
            fprintf(stderr, "send函数出错，data发送失败\n");
            return -1;
        }
        else
        {
            sent += result;
        }
    }
    return 0;
}

int recv_msg(int sockfd, struct ftpmsg *msg)
{
    int result;

    // 接收type
    result = recv(sockfd, &msg->type, sizeof(int), 0);
    if (result == -1)
    {
        if (errno == EWOULDBLOCK)
        {
            // 没有收到数据
            printf("没有收到数据\n");
            return 1;
        }
        fprintf(stderr, "recv函数出错，type接收失败\n");
        return -1;
    }
    // printf("< 接收信息：type = %d", msg->type);

    // 接收len
    result = recv(sockfd, &msg->len, sizeof(int), 0);
    if (result == -1)
    {
        fprintf(stderr, "recv函数出错，len接收失败\n");
        return -1;
    }
    // printf(", len = %d", msg->len);

    // 接收data
    if (msg->len == 0)
    {
        msg->data = NULL;
    }
    else
    {
        msg->data = malloc(msg->len);
        if (msg->data == NULL)
        {
            fprintf(stderr, "malloc函数出错，接收消息时动态内存分配失败\n");
            return -1;
        }
        else
        {
            // printf("分配了%d位\n", msg->len);
        }

        int recvd = 0;           // 总共已接收的位数
        while (recvd < msg->len) // 循环直到data接收完毕
        {
            result = recv(sockfd, msg->data + recvd, msg->len - recvd, 0);
            if (result == -1)
            {
                fprintf(stderr, "recv函数出错，data接收失败\n");
                return -1;
            }
            else
            {
                recvd += result;
            }
        }
    }
    // printf(", data = %-30.30s\n", msg->data);
    return 0;
}

int send_simple(int sockfd, enum ftpmsg_type type)
{
    struct ftpmsg msg;
    msg.type = type;
    msg.len = 0;
    msg.data = NULL;
    return send_msg(sockfd, &msg);
}

int send_file(int sockfd, char *path)
{
    char buf[2048], filename[MAX_LENGTH];
    int i = 0, fd, nread;
    long sent = 0;
    struct stat statbuf;
    struct ftpmsg msg;

    // 打开源文件
    fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "open函数出错，打开源文件失败\n");
        return -1;
    }

    // 读取源文件属性
    if (stat(path, &statbuf) == -1)
    {
        fprintf(stderr, "stat函数出错，读取源文件属性失败\n");
        return -1;
    }

    // 发送文件名
    msg.type = FILE_NAME;
    msg.data = malloc(sizeof(path));
    if (msg.data == NULL)
    {
        fprintf(stderr, "malloc函数出错，发送文件内容失败\n");
        return -1;
    }
    file_name(msg.data, path);
    file_name(filename, path);
    msg.len = strlen(msg.data) + 1;
    send_msg(sockfd, &msg);
    // printf("发送文件名成功\n");

    // 发送st_mode
    msg.type = FILE_MODE;
    sprintf(msg.data, "%d", statbuf.st_mode);
    msg.len = strlen(msg.data) + 1;
    send_msg(sockfd, &msg);
    // printf("发送st_mode成功\n");

    // 发送文件大小
    msg.type = FILE_SIZE;
    sprintf(msg.data, "%ld", statbuf.st_size);
    msg.len = strlen(msg.data) + 1;
    send_msg(sockfd, &msg);
    // printf("发送文件大小成功\n");

    // 接收答复，文件是否创建成功
    recv_msg(sockfd, &msg);
    if (msg.type == FAILURE)
    {
        fprintf(stderr, "open函数出错，创建目标文件失败\n");
        return -1;
    }

    // 发送内容
    printf("正在发送文件%s：\n", filename);
    while ((nread = read(fd, buf, sizeof(buf))) != 0)
    {
        if (nread == -1)
        {
            fprintf(stderr, "read函数出错，读取源文件失败\n");
            return -1;
        }
        msg.type = FILE_DATA;
        msg.len = nread;
        msg.data = buf;
        send_msg(sockfd, &msg);
        sent += nread;
        for (i = i; i < ((float)sent / statbuf.st_size * 50); i++)
        {
            putchar('#');
            fflush(stdout);
        }
    }

    close(fd);

    // 发送结束标志
    send_simple(sockfd, FILE_END);

    // 接收答复，文件是否发送成功
    recv_msg(sockfd, &msg);
    if (msg.type == SUCCESS)
    {
        printf("\n%s发送成功（已发送%ld/%ld字节）\n", filename, atol(msg.data), statbuf.st_size);
    }
    else
    {
        printf("\n%s发送失败（已发送%ld/%ld字节）\n", filename, atol(msg.data), statbuf.st_size);
    }
}

int recv_file(int sockfd)
{
    struct ftpmsg msg;
    char *filename;
    mode_t mode;
    int i = 0, fd, nwrite;
    long size, recvd = 0;

    // 接收文件名
    recv_msg(sockfd, &msg);
    filename = msg.data;

    // 接收st_mode
    recv_msg(sockfd, &msg);
    mode = atoi(msg.data);

    // 接收文件大小
    recv_msg(sockfd, &msg);
    size = atol(msg.data);

    // 创建文件，回复是否成功
    fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, mode % 01000);
    if (fd == -1)
    {
        fprintf(stderr, "open函数出错，创建目标文件失败\n");
        send_simple(sockfd, FAILURE);
        return -1;
    }
    else
    {
        send_simple(sockfd, SUCCESS);
    }

    // 接收内容
    printf("正在接收文件%s：\n", filename);
    recv_msg(sockfd, &msg);
    while (msg.type != FILE_END)
    {
        if ((nwrite = write(fd, msg.data, msg.len)) == -1)
        {
            fprintf(stderr, "write函数出错，写入目标文件失败\n");
            printf("\n%s接收失败（已接收%ld/%ld字节）\n", filename, recvd, size);
            msg.type = FAILURE;
            msg.len = sizeof(long);
            sprintf(msg.data, "%ld", recvd);
            send_msg(sockfd, &msg);
            close(fd);
            return -1;
        }
        else
        {
            recvd += nwrite;
            for (i = i; i < ((double)recvd / size * 50); i++)
            {
                putchar('#');
                fflush(stdout);
            }
            recv_msg(sockfd, &msg);
        }
    }
    msg.type = SUCCESS;
    msg.data = malloc(sizeof(long));
    sprintf(msg.data, "%ld", recvd);
    msg.len = strlen(msg.data);
    send_msg(sockfd, &msg);
    close(fd);
    printf("\n%s接收成功（已接收%ld/%ld字节）\n", filename, recvd, size);
    return 0;
}