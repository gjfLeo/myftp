#ifndef _MYFTP_CMD_SERVER_H_
#define _MYFTP_CMD_SERVER_H_

#include "cmd.h"

#include "cmd_server.h"
#include "file.h"
#include "header.h"
#include "msg.h"

int c_login(int sockfd, char *username);
int c_mkdir(int sockfd, char *dir);
int c_rmdir(int sockfd, char *dir);
int c_pwd(int sockfd);
int c_cd(int sockfd, char *dir);
int c_ls(int sockfd, char *dir);
int c_put(int sockfd);
int c_get(int sockfd, char *path);

#endif