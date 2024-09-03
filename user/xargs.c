#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAX_LINE_LEN 80

int
main(int argc, char** argv)
{
    // 读取 xargs 之后的参数,存入字符指针数组 args 中
    char* args[MAXARG];
    for(int i = 1; i < argc; i++)
    {
        args[i - 1] = argv[i]; // 最多存到args[argc - 2]
    }

    // 把args[agrc - 1], 给管道输送进来的每行参数
    args[argc - 1] = malloc(MAX_LINE_LEN);
    args[argc] = 0;

    // 每行都存到args[argc - 1],后面的行会覆盖前面的行
    while(gets(args[argc - 1], MAX_LINE_LEN))
    {
        // 读到了最后一行的下一行
        // 已读完, 不加这行在使用 xargs 会报错usertrap()
        if(args[argc - 1][0] == 0)  break; 
        // 将 gets 引起的换行符转为字符串结尾的0
        if(args[argc - 1][strlen(args[argc - 1]) - 1] == '\n')
            args[argc - 1][strlen(args[argc - 1]) - 1] = 0;
        if(fork() == 0)
        {
            exec(argv[1], args);
            fprintf(2, "usage: find <directory> <filename>\n");
        }
        else 
        {
            wait(0);
        }
    }

    exit(0);
}