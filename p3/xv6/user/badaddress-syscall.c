#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	/* In between heap and stack */
	char *p = (char *) (10 * 1024);
	if(pipe((int *)p) == -1) {
		printf(1, "no Segfault\n");
	}
	exit();
}
