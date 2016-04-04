#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	int i;
	
	/* Checking where the heap is */
	int *x = (int *)malloc(sizeof(int));
        printf(1, "address of x : %d\n", x);	
	
	int y = 1;
	printf(1, "address of y : %d\n", &y); 	

	int size = (int)(&y - x);
	
	int array[size];
	for(i = 0; i < size; ++ i) {
		array[i] = i;
	}
	/* Check for allocation of second page table for stack */
	printf(1, "first address : %d\n", array);
	printf(1, "last address : %d\n", array + size - 1);
	printf(1, "last element : %d\n", *(array + size - 1));
	
	/* Will fail beccause the stack is overallocated */
	exit();
}
