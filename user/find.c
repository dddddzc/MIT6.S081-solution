#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

// struct stat {
//   int dev;     // File system's disk device
//   uint ino;    // Inode number
//   short type;  // Type of file
//   short nlink; // Number of links to file
//   uint64 size; // Size of file in bytes
// };

// struct dirent {
//   ushort inum;
//   char name[DIRSIZ];
// };

void
find(char *path, const char* filename)
{
    char buf[512];     // 会将path放到buf中,进行后续处理(例如添加下一级目录)
    char* p;           // 指针,用来跟踪 buf
    int fd;            // 打开文件时的 file descriptor
    struct dirent de;  // 目录中,每个条目的长度和名字
    struct stat st;    // 打开文件的状态(目录也是文件!!!)

    if((fd = open(path, 0)) < 0){
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // find 的第一个参数必须是目录
    if(st.type != T_DIR){
        fprintf(2, "usage: find <directory> <filename>\n", path);
        return;
    }
    
    strcpy(buf, path);     // 将path字符串拷贝到buf中
    p = buf + strlen(buf); // 通过 + 运算符将指针 p 移动到 buf 中字符串的末尾
    *p++ = '/';            // 在 p 指向的位置放置一个'/', 然后将p指针向前挪动一位

    // 打开一个目录并读取，会逐个获取 dirent 结构体, 即de
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0) continue;   // 跳过空目录条目
        memmove(p, de.name, DIRSIZ); // 将 de.name 复制到 p 指向的位置，最多复制 DIRSIZ 个字符
        p[strlen(de.name)] = 0;      // 添加结束的字符0

        // 查看新目录 buf 的stat
        if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
        }

        if(strcmp(filename, p) == 0){ // p指向的是最后一个'/'后的字符串内容,说明找到了
            printf("%s\n", buf);
        }
        // 在递归前保证 buf 指向的是目录, 并且当前目录 p 不是 '.' 和 '..'
        else if(st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0){
            find(buf, filename);
        }
    }

    close(fd);
}


int 
main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(2, "usage: find <directory> <filename>\n");
        exit(1);
    }
    // argv[1] : directory
    // argv[2] : filename
    find(argv[1], argv[2]);
    exit(0);
}