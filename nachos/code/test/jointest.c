#include "syscall.h"

int
main() {
SpaceId newProc,newProc2;
int result = 0;

newProc = Exec("../test/exectest", 0, 0, 1);
Join(newProc);

newProc2 = Exec("../test/exittest", 0, 0, 1);
result = Join(20);
Exit(result);

}
