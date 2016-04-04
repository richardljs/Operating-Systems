#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "../include/pstat.h"

int main (int argc, char *argv[])
{
  int i;
  int x = 0;
  if (argc != 2) {
    exit();
  }
  for (i = 0; i < atoi(argv[1]); i++)
    x += i;
  struct pstat *p = NULL;
  getpinfo(p);
  printf(1, "pid = %d\n", p->pid);
  exit();
  return x;
}
