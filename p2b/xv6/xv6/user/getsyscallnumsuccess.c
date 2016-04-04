#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  printf(1, "Number of Successful Syscalls: %d\n", getsyscallnumsuccess());
  exit();
}
