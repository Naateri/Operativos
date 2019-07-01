//#include <iostream>
#include <stdio.h>
#include <time.h> 
#include <stdlib.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <signal.h>
#include <queue>
#include <thread>

//g++ pipe_write.cpp -o pipe_write -std=c++11 -lpthread

std::queue<int> le_numbers;

int rdtsc(){
	__asm__ __volatile__("rdtsc");
}

void gen_num(){
	long RandNum = 0;
	srand(rdtsc());
	RandNum = rand()% 100;
	le_numbers.push(RandNum);
	printf("Queue size: %d\n", le_numbers.size());
}

void print_queue(){
	std::queue<int> temp (le_numbers);
	printf("Queue: ");
	while(!temp.empty()){
		int num = temp.front();
		temp.pop();
		printf("%d ", num);
	}
	printf("\n");
}

bool check(char* num_generated){
	int fd;
    char * myfifo = "/tmp/other_pipe";
    char buf[80];
    fd = open(myfifo,O_RDONLY);
    while (read(fd, buf, 80) == -1){
        ;
    }
    /*if (strcmp(num_generated, buf) == 0) return 1;
    else return 0;*/
    close(fd);
    if (strcmp("OK", buf) == 0){
   		return 1;
    }
    //return 1;
  	return 0;

}

void send_pipe(){
	int fd; 
	char randNumbers[100];
	if (!le_numbers.empty()){
		//print_queue();
		long RandNum = le_numbers.front();
		le_numbers.pop();
    	// FIFO file path
    	char* myfifo = "/tmp/myfifo";  //all numbers
    	mkfifo(myfifo, 0666); 

    	// Open FIFO for write only         
    	sprintf (randNumbers, "%ld", RandNum);
    	printf("Number getting passed: %s\n", randNumbers);
	
		fd = open(myfifo, O_WRONLY);
    	write(fd, randNumbers, strlen(randNumbers)+1); 
    	close(fd);
    	printf("Waiting for Confirmation\n");
    	/*if (!check(randNumbers)){
    		printf("Confirmation is wrong\n");
    		exit(0);
    	}*/
	}
}

void signhandler(int signum){
	if (signum == 2){
		std::thread t1(gen_num);
		t1.join();	
		//send_pipe();
		//unlink(myfifo);
		//sleep(1);
		//exit(0);	
	}
}
  
int main() 
{ 
	std::thread t1(signal,SIGINT, signhandler);
	t1.detach();	
	while (1){
		//signal(SIGINT, signhandler);
		std::thread t2(send_pipe);
		t2.join();
		//send_pipe();
		/*printf("Waiting for sgnal (hint: kill -2 pid)\n");
		printf("Second hint: my pid is: %d\n", getpid());*/
		//sleep(1);
	}
    return 0; 
}
