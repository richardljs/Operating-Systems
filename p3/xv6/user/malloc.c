#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	int *array;
	array = (int *)malloc(sizeof(int) * 50);
	array[25] = 0;
	printf(1, "address of first array : %d\n", array);
	char *array2;
	array2 = (char *)malloc(sizeof(char) * 25);
	array2 = "abcdefg";
	printf(1, "address of second array : %d\n", array2);	
	exit();	
}
