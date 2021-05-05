#include <stdio.h>

int main(){
	long z;
	long* y;
	long* x;
	z = 0;
	y = 0;
	x = 0;
	z = 42;
	x = &z;
	y = x;
	z = *x+1;
	printf("%ld\n", z);
	return 0;
}
