
#include "syscall.h"
int 
main()
{
int result = 1000;
    result = Exec("../test/../test/../test/../test/../test/../test/../test/../test/../test/../test/../test/../test/exittest", 0, 0, 0);
    result = Exec("../test/../test/../test/../test/../test/../test/../test/../test/../test/../test/../test/../test/exittest", 0, 0, 0);

    Exit(result);
}
