#include "syscall.h"

int
main()
{
    int result = 1000;
    int argc = 3;
    char* argv[argc];

    argv[0] = 'a';
    argv[1] = 'b';
    argv[2] = 'c';
  
    result = Exec("../test/exetest5", argc, argv, 0);
    Exit(result);
}

