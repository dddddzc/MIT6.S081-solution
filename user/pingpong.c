#include "kernel/types.h"
#include "user/user.h"

int 
main(void)
{
    char buf[4];
    int p[2]; // p[1] for read, p[0] for write
    pipe(p);

    if(fork() == 0) // 子进程
    {
        close(p[1]);
        read(p[0], buf, 4);
        printf("%d: received ping\n", getpid());
        close(p[0]);
    }
    else    // 父进程
    {
        close(p[0]); // parent close 0, because it would write to p[1], then child read p[1]
        write(p[1], buf, 4); //  a byte
        wait(0); // 保证ping 和 pong 的顺序
        printf("%d: received pong\n", getpid());
        close(p[1]);
    }

    exit(0);
}