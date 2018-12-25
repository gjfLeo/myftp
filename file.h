#ifndef _MYFTP_FILE_H_
#define _MYFTP_FILE_H_

char *full_path(char *path);
char *file_name(char *filename, const char *path);
int copy_file(char *src_file, char *dest_path);
int create_dir(char *dir_name);
int delete_dir(char *dir_name);

#endif