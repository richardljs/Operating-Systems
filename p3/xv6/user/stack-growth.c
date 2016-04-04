#include "types.h"
#include "user.h"

int fibo(int);

int
main(int argc, char *argv[])
{
	printf(1, "fibonacci series with recursion for n = 10 : %d\n", fibo(10));
	exit();	
}

int
fibo(int n)
{
	if(n == 0) return 0;
	if(n == 1) return 1;
	else return fibo(n-1) + fibo(n-2);
}
