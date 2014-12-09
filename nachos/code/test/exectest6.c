#include "syscall.h"

int
main(int argc, char *argv[])
{
    int result = 1000;
    int i;
    int sum = 0;

    for (i = 0; i< argc; i++)
        sum = sum + (int) argv[i];
}

