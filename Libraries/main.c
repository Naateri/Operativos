#include <stdio.h>
#include "my_math.h"
#include "names.h"

int main(int argc, char* argv[]){
	
	setName("Renato2");
	print_name();
	
	
	set_num(100);
	set_num2(20);
	printf("mcd(100,20) = %d \n",mcd());
	
	set_num(5);
	printf("5! = %d \n", factorial());		
	
	return 0;
}
