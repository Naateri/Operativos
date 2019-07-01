/*
 * shm-client - client program to demonstrate shared memory.
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

//ipc
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>

#define SHMSZ 27
#define MAXSIZE 128 //IPC 

struct msg_buf //ipc
{
    long    mtype;
    char    mtext[MAXSIZE];
};

void die(char *s) //IPC
{
  perror(s);
  exit(1);
}


main()
{	
    int shmid;
    key_t key;
    char *shm, *s;

    /*
     * We need to get the segment named
     * "5678", created by the server.
     */
    key = 5678;

    /*
     * Locate the segment.
     */

    while (1){

    while ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        /*perror("shmget");
        exit(1);*/
        printf("Waiting for shared memory creation\n");
        sleep(1);
    }

    //printf("shmid: %d\n", shmid);

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    //printf("First shm value recieved: %s\n", shm);

    /*
     * Now read what the server put in the memory.
     */

    while(*shm == '*'){
    	//printf("Waiting for a value\n");
    	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        	perror("shmat");
        	exit(1);
    	}
    	//printf("cur shm: %s\n", shm);
    	sleep(1);
    }        

    /*
     * Finally, change the first character of the 
     * segment to '*', indicating we have read 
     * the segment.
     */
    printf("%s\n", shm);
    printf("Read value on the shared memory: %s\n", shm);

    FILE *f = fopen("impares.txt", "a");
    fprintf(f, "%s ", shm);
    fclose(f);

    *shm = '*';



    //IPC
	}
}