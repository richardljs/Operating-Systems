#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
        char *p = 0;
	if(pipe((int *)p) < 0) {
		printf(1, "No Segfault\n");
	}
        exit();
}
