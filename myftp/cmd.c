#include "cmd.h"
#include "header.h"

// 从line中分离第一个以空格分开的命令，存入token，并将其从line中移除
int gettoken(char *token, char *line)
{
    int i = 0; // 记录token的位数
    // 移除line开头的空格
    while (*line == ' ')
    {
        strcpy(line, line + 1);
    }
    if (*line == '\"')
    {
        // 特殊情况：第一个单词由引号开头，则将第二个引号之前的内容移到token
        strcpy(line, line + 1);
        // 遇到第二个引号前，把每个字符移入token
        while (*line != '\"' && *line != '\0')
        {
            *token++ = *line;
            i++;
            strcpy(line, line + 1);
        }
        strcpy(line, line + 1); // 把第二个引号移除
    }
    else
    {
        // 将第一个词移入token
        while (*line != ' ' && *line != '\"' && *line != '\0')
        {
            *token++ = *line;
            i++;
            strcpy(line, line + 1);
        }
    }
    // 给token添加结束符号
    *token++ = '\0';
    if (i == 0)
    {
        return -1;
    }
    // 移除第一个词后的空格
    while (*line == ' ')
    {
        strcpy(line, line + 1);
    }
    return i;
}