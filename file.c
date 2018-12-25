#include "file.h"
#include "cmd.h"
#include "header.h"

// 将路径转换为绝对路径
char *full_path(char *path)
{
    // printf("相对路径：%s\n", path);
    // 如果输入的是相对路径（首字符不是'/'）则转换
    // 绝对路径（path的新值） = 当前路径 + "/" + 相对路径（path原来的值）;
    if (path[0] != '/')
    {
        char *cwd;
        cwd = getcwd(NULL, 0);
        strcat(cwd, "/");
        strcat(cwd, path);
        strcpy(path, cwd);
    }
    // printf("绝对路径：%s\n", path);
    return path;
}

// 从文件路径获取文件名
char *file_name(char *filename, const char *path)
{
    if (strchr(path, '/') == NULL)
    {
        strcpy(filename, path);
    }
    else
    {
        strcpy(filename, strrchr(path, '/') + 1);
    }
    return filename;
}

int copy_file(char *src_file, char *dest_path)
{
    char block[1024];
    int in, out;
    int nread;
    DIR *dp;

    // 读取源文件
    in = open(src_file, O_RDONLY);
    if (in == -1)
    {
        perror("源文件读取失败\n");
        return -1;
    }
    // 从源文件路径中获取源文件的文件名
    char *ret1 = strrchr(src_file, '/');
    char *ret2;
    strncpy(ret2, ret1 + 1, strlen(ret1) - 1);

    // 如果目标路径不存在
    if ((dp = opendir(dest_path)) == NULL)
    {
        if (create_dir(dest_path) == -1)
        {
            perror("目标路径创建失败\n");
            return -1;
        }
    }

    out = open(dest_path, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    if (out == -1)
    {
        perror("打开目标文件失败\n");
        return -1;
    }

    while ((nread = read(in, block, sizeof(block))) > 0)
    {
        if (write(out, block, nread) < 0)
        {
            perror("写入目标文件失败\n");
            return -1;
        }
    }
    return 0;
}

int create_dir(char *dir_name)
{
    // 保存当前工作路径
    char pwd[MAX_LENGTH];
    getcwd(pwd, MAX_LENGTH);

    char *path = full_path(dir_name);

    DIR *dp;
    char *dir;
    dir = strtok(path, "/");
    chdir("/");
    while (dir != NULL)
    {
        if ((chdir(dir)) == -1)
        {
            mkdir(dir, 0777);
            // printf("创建目录%s/%s成功\n", getcwd(NULL, 0), dir);
            chdir(dir);
        }
        dir = strtok(NULL, "/");
    }

    // 切换到原工作路径
    chdir(pwd);
    return 0;
}

int delete_dir(char *dir_name)
{
    // 保存当前工作路径
    char pwd[MAX_LENGTH];
    getcwd(pwd, MAX_LENGTH);

    char *dir = full_path(dir_name);

    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if ((dp = opendir(dir)) == NULL)
    {
        fprintf(stderr, "无法打开目录：%s\n", dir);
        return 0;
    }
    chdir(dir);
    // printf("已进入目录%s\n", getcwd(NULL, 0));
    while ((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            // 删除目录（忽略.和..）
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;
            if (strchr(entry->d_name, '/') != NULL)
            {
                // printf("%s被跳过\n", entry->d_name);
                continue;
            }
            delete_dir(entry->d_name);
        }
        else
        {
            // 删除文件
            if (unlink(entry->d_name) == 0)
            {
                // printf("已删除文件%s/%s\n", getcwd(NULL, 0), entry->d_name);
            }
            else
            {
                fprintf(stderr, "删除文件失败：%s/%s\n", getcwd(NULL, 0), entry->d_name);
            }
        }
    }
    // printf("当前目录已清空，即将返回上级目录\n");
    chdir("..");
    // printf("已进入目录%s\n", getcwd(NULL, 0));
    closedir(dp);
    rmdir(dir);
    // printf("已删除目录%s\n", dir);

    // 切换到原工作路径
    chdir(pwd);
    return 0;
}