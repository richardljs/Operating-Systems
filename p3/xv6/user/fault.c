#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	char *p = (char *) (300 * 1024);
	/* Try accessing address in between heap and stack */
        *p = 'a';
	exit();
}
