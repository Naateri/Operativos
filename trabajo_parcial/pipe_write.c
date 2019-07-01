#include <stdio.h>
#include <time.h> 
#include <stdlib.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <signal.h>

void signhandler(int signum){
	if (signum == 2){
		int fd; 
    	long RandNum = 0;
    	char randNumbers[100];
    	// FIFO file path 
    	char* myfifo = "/tmp/myfifo";  //odd numbers
    	char* myfifo2 = "/tmp/myfifo2"; //even numbers
  
    	mkfifo(myfifo, 0666); 
    	mkfifo(myfifo2, 0666);

		srand(time(NULL));

    	// Open FIFO for write only         
        RandNum = rand()% 100;
        sprintf (randNumbers, "%ld", RandNum);
		printf("Number generated: %s\n", randNumbers);
		if ( RandNum & 1 ) fd = open(myfifo, O_WRONLY); //odd numbers
		else fd = open(myfifo2, O_WRONLY); //even numbers
        write(fd,randNumbers, strlen(randNumbers)+1); 
    	close(fd);
		//unlink(myfifo);
		sleep(1);
		//exit(0);	
	}
}
  
int main() 
{ 
    signal(SIGINT, signhandler);
	while (1){
		/*printf("Waiting for signal (hint: kill -2 pid)\n");
		printf("Second hint: my pid is: %d\n", getpid());*/
		sleep(1);
	}
    return 0; 
}
