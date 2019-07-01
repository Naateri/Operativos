#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdlib.h>
#include <fcntl.h>
#include <sys/msg.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <sys/shm.h>

#define MAXSIZE 64

#define MAX_MSG_SIZE 40

#define SHMSZ 27


void die(char *s)
{
  perror(s);
  exit(1);
}

struct msgbuf
{
    long    mtype;
    char    mtext[MAXSIZE];
};


void main()
{
    int msqid;
    key_t key;
    struct msgbuf rcvbuffer;

    key = 1234;

    while(1){

    	while ((msqid = msgget(key, 0666)) < 0){
      		//die("msgget()");
      		printf("Waiting for IPC Creation\n");
      		sleep(1);
    	}


     //Receive an answer of message type 1.
    	if (msgrcv(msqid, &rcvbuffer, MAXSIZE, 1, 0) < 0)
      		die("msgrcv");


    	printf("message recieved: %s\n", rcvbuffer.mtext);


    /////////SHARED MEMORY////////
    	char c;
    	int shmid;
    	key_t key;
		char *shm, *s; //shm
	
    ////////////////////////////////////// 

    //shared memory (based on: shm_server.c)	

	/*
     * We'll name our shared memory segment
     * "5678".
     */
    	key = 5678;

    /*
     * Create the segment.
     */
    	if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
    		perror("shmget");
    		exit(1);
    	}	

    //printf("shmid: %d\n", shmid);

    /*
     * Now we attach the segment to our data space.
     */
    	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
     		perror("shmat");
      		exit(1);
    	}

    	*shm = '*'; //so shm waits for a value


		//looping
		s = shm;
            
        char* rbuf = rcvbuffer.mtext;            
        for (rbuf; *rbuf != '\0'; rbuf++){
        	*s = *rbuf;
        	s++;        		
        }
        *s = NULL;            
            /*printf("First char rbuf %c\n", *rbuf);
            printf("Second char rbuf %c\n", *(rbuf+1));
            if ( *(rbuf+2) == '\0') printf("Third char rbuf %c\n", *(rbuf+2));*/
            //s = readbuffer;                     
            //shm = s;           
        printf("value put on shared memory: %s\n", shm);       
            
        while (*shm != '*'){
        	//printf("Waiting for the shared memory to be read\n");        
		    sleep(1);
		}

	}
    exit(0);
}
