#ifndef _MYFTP_CMD_CLIENT_H_
#define _MYFTP_CMD_CLIENT_H_

#include "cmd.h"

int client_login(int sockfd);
int client_lmkdir(char *dir);
int client_lrmdir(char *dir);
int client_lpwd();
int client_lcd(char *dir);
int client_dir(char *dir);
int client_mkdir(int sockfd, char *dir);
int client_rmdir(int sockfd, char *dir);
int client_pwd(int sockfd);
int client_cd(int sockfd, char *dir);
int client_ls(int sockfd, char *dir);
int client_put(int sockfd, char *path);
int client_get(int sockfd, char *path);
int client_help();

#endif