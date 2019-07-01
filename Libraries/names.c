#include <stdio.h>

char* o_name;

void setName(char* name){
	o_name = name;
}

void print_name(){
	printf("my name is %s\n", o_name);
}

void __attribute__((constructor)) initLibrary(void){
	printf("Initializing static library\n");
	o_name = "Renato";
}

void __attribute__((destructor)) cleanUpLibrary(void){
	printf("static library destroyed\n");
}
