long foo(long * arg){ 
	long x = *arg; // arg does not outlive foo()
	//long *z = &x; // z does not outlive foo()
	return x;
}

int main(){
	long x;
	long *y;
	x = 0;
	y = &x;
	x = foo(y);
	return 0;
}
