//Autor: Renato Luis Postigo Avalos
//compilacion: gcc prg2.c -o prg2.exe

//recieve data
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXSIZE 64

struct msg_buf {
    long type;
    char text[MAXSIZE];
};

int main(){
	int msqid, pid;
    key_t key;
    struct msg_buf recieve_buffer;
	
	pid = fork();	
	
	if (pid == 0){
		printf("Child process\n");
		key = 1234;
		//system("echo hijo");

		msqid = msgget(key, 0666);
		printf("Waiting for message\n");
		msgrcv(msqid, &recieve_buffer, MAXSIZE, 1, 0);
	
    	printf("Message recieved: %s\n", recieve_buffer.text);
		//exit(0);
		return 0;

	} else {
		printf("Father process\n");
		key = 1234;

		msqid = msgget(key, 0666);
		printf("Waiting for message\n");
		msgrcv(msqid, &recieve_buffer, MAXSIZE, 1, 0);

   		printf("Message recieved: %s\n", recieve_buffer.text);
		//exit(0);
		return 0;
	}
	return 0;
}
