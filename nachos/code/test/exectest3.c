#include "syscall.h"

int
main()
{
    int result = 1000;
    char path[3];
    path[0] = 'a';
    path[1] = 'b';
    path[2] = 'c'; 
    result = Exec(path, 0, 0, 0);
    Exit(result);
}
