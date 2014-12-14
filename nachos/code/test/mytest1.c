/*
 * exectest.c
 *
 * Exec a simple program.  Return the result of Exec.
 */

#include "syscall.h"

int
main()
{   int i =1;
    int result = 1000;
    while(i<2){
    result = Exec("../test/GoodLocality", 0, 0, 0);
    result = Exec("../test/exectest", 0, 0, 0);
    i++;
}
    Exit(result);
}
