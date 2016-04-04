#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  printf(1, "Total Number of Syscalls: %d\n", getsyscallnumtotal());
  exit();
}
