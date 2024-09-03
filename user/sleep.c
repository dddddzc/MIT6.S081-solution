#include "kernel/types.h"
#include "user/user.h"

int 
main(int argc, char *argv[])
{
    if(argc != 2){
        fprintf(2, "usage: sleep <time>\n");
        exit(1);
    }

    int sleep_duration = atoi(argv[1]);
    sleep(sleep_duration);
    exit(0);
}