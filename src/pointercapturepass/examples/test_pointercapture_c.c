#include <stdio.h>

long bar(){
	long * x = malloc(sizeof(long));
	*x = 99;
	printf("%ld\n", *x+1);
	return 0;
}

long * foo(){
	long x;
	long * y = malloc(sizeof(long));
	*y = 42;
	x = bar();
	return y;

}

int main(){
	long* x;
	x = 0;
	x = foo();
	printf("%ld\n", *x);
	return 0;
}
