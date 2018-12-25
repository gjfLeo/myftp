#ifndef _MYFTP_MSG_H_
#define _MYFTP_MSG_H_

#define DEFAULT_PORT 12345
#define DEFAULT_IP "127.0.0.1"

enum ftpmsg_type // FTP消息类型
{
    DEFAULT,
    LOGIN,       // 登录信息（客户端发送的用户名或密码）
    LOGIN_PASS,  // 登录验证通过（服务器接收用户名或密码后发送）
    LOGIN_ERROR, // 登录验证错误（服务器接收用户名或密码后发送）
    C_MKDIR,     // 客户端mkdir命令
    C_RMDIR,     // 客户端rmdir命令
    C_PWD,       // 客户端pwd命令
    C_CD,        // 客户端cd命令
    C_LS,        // 客户端ls命令
    C_GET,       // 客户端get命令
    C_PUT,       // 客户端put命令，传送文件名，等待传输文件
    C_QUIT,      // 客户端退出
    FILE_,       // get和put命令中传输文件内容（为了和系统的FILE区分，加了个下划线）
    FILE_NAME,
    FILE_MODE,
    FILE_SIZE,
    FILE_DATA,
    FILE_END, // 表示文件传输完毕
    S_SUCCESS,
    S_FAILURE,
    SUCCESS,
    FAILURE,
};

struct ftpmsg // FTP消息
{
    enum ftpmsg_type type; // 消息类型
    int len;               // 数据长度
    char *data;            // 数据内容
};

int send_msg(int sockfd, struct ftpmsg *msg);
int recv_msg(int sockfd, struct ftpmsg *msg);

int send_simple(int sockfd, enum ftpmsg_type type);

int send_file(int sockfd, char *path);
int recv_file(int sockfd);

#endif