#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	char *p = (char *) ((640 * 1024) - 4096);
	printf(1, "%d\n", p);
	*p = 'a';
	printf(1, "%c\n", *p);

	int q = 1;
	printf(1, "address of the first variable(an int): %d\n", &q);
	char x = 'b';
	printf(1, "address of the second variable(a char): %d\n", &x);
	int r = 10000;
	printf(1, "address of the third variable(an int): %d\n", &r);
	exit();
}
