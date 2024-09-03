#include "kernel/types.h"
#include "user/user.h"

void primes(int* p)
{
    // pipe[0] 是管道的read端,用于从pipe中读取数据
    // pipe[1] 是管道的write端,用于向pipe中写入数据
    close(p[1]); // 关闭write端,因为每个进程只需要从旧管道中读,再向新管道中写

    int curr_prime;
    if(read(p[0], &curr_prime, 4) == 0) // 旧管道为空,素数筛空了,关闭p[0],递归的终点
    {
        close(p[0]);
        exit(0);
    }

    // 旧管道非空,print之后,开新管道
    printf("prime %d\n", curr_prime); // 打印旧管道中的第一个素数,在第一次递归中为2
    int new_p[2];
    pipe(new_p);     

    int pid = fork();
    if(pid == 0) // 开出来的子进程
    {
        primes(new_p);
    }
    else if(pid > 0) // 当前的父进程,开筛!然后写入
    {
        int num;
        while(read(p[0], &num, 4))
        {
            if(num % curr_prime) write(new_p[1], &num, 4); // 除不尽,写入
        }
        close(p[0]);     // 关闭旧管道的read端,因为父进程已经读完毕
        close(new_p[1]); // 关闭新管道的write端,因为父进程已经写完毕
        wait(0);
    }
    else
    {
        fprintf(2, "fork error!\n0");
    }
}

int main()
{
    int p[2];
    pipe(p);

    for(int i = 2; i <= 35; i++)
    {
        write(p[1], &i, 4); // 一个 int 占4个字节
    }

    primes(p); // 交给primes的是读写都开放的管道
    exit(0);
}