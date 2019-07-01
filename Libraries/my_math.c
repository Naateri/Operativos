#include <stdio.h>

int test_num, test_num2;

void set_num(int num){
	test_num = num;
}

void set_num2(int num){
	test_num2 = num;
}

int mcd(){
	int r, a, b;
	a = test_num;
	b = test_num2;
	while (b!=0){
		r = a%b;
		a = b;
		b = r;
	}
	return a;
}

int factorial(){
	if (test_num <= 1) return 1;
	int res = 1;
	for (int i = 2; i <= test_num; i++){
		res *= i;
	}
	return res;
}

void __attribute__((constructor)) initLibrary(void){
	printf("Initializing dynamic library\n");
	test_num = 2;
	test_num2 = 1;
}

void __attribute__((destructor)) cleanUpLibrary(void){
	printf("dynamic library destroyed\n");
}
