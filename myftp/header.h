#ifndef _MYFTP_HEADER_H_
#define _MYFTP_HEADER_H_

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
// char *strcat(char *dest, const char *src);
// char *strcpy(char *dest, const char *src);
// char *strncpy(char *dest, const char *src, size_t n);
// char *strtok(char *str, const char *delim);

#include <unistd.h>
// size_t write(int fildes, const void *buf, size_t nbytes);
// size_t read(int fildes, void *buf, size_t nbytes);
// int chdir(const char *path);
// int close(int fildes);
// char *getcwd(char *buf, size_t size);
// int rmdir(const char *path);
// int unlink(const char *path);

#include <dirent.h>
// DIR *opendir(const char *name);
// struct dirent *readdir(DIR *dirp);
// int closedir(DIR *dirp);

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
// int socket(int domain, int type, int protocol);
// int connect(int socket, const struct sockaddr *address, size_t address_len);
// int open(const char *path, int oflags, mode_t mode);
// int lstat(const char *path, struct stat *buf);
// int mkdir(const char *path, mode_t mode);
// int send(SOCKET s, const char *buf, int len, int flags);
// int recv(SOCKET s, char *buf, int len, int flags);      （flags正常使用时取0）

#include <netinet/in.h>
// unsigned long int htons(unsigned short int hostshort);

#include <arpa/inet.h>
// in_addr_t inet_addr(const char *cp);

#include <errno.h>

#define MAX_LENGTH 1000 // 字符串长度上限
#define MAX_ARGC 10     // 命令参数个数上限+1

#endif