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
#include <thread>

#include <list>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define SHMSZ 27
#define MAXSIZE 128 //IPC 

//g++ shm_client.cpp -o shm_client -std=c++11 -lpthread

struct msg_buf //ipc
{
    long    mtype;
    char    mtext[MAXSIZE];
};

std::list<int> le_numbers; //queue ordered by min

void die(char *s) //IPC
{
  perror(s);
  exit(1);
}

void recieve_message(){
    int shmid;
    key_t key;
    char *shm, *s;
    //We need to get the segment named "5678", created by the server.
    key = 5678;
    int num;

    //Locate the segment.
    
    while ( (shmid = shmget(key, SHMSZ, 0666)) < 0) {
        /*perror("shmget");
        exit(1);*/
        printf("Waiting for shared memory creation\n");
        //sleep(1);
    }

    //printf("shmid: %d\n", shmid);

    /*
     * Now we attach the segment to our data space.
     */

    shm = (char*)shmat(shmid, NULL, 0);

    if ( shm == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    //printf("First shm value recieved: %s\n", shm);

    //Now read what the server put in memory

    while(*shm == '*'){
        //printf("Waiting for a value\n");
        /*if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
            perror("shmat");
            exit(1);
        }*/
    }        

    /*
     * Finally, change the first character of the 
     * segment to '*', indicating we have read 
     * the segment.
     */
    printf("%s\n", shm);

    char* temp = new char[3];
    temp[0] = *shm;
    temp[1] = *(shm+1);
    temp[2] = '\0';

    num = atoi(temp);

    //le_numbers.push_back(temp);
    le_numbers.push_back(num);

    le_numbers.sort();
    le_numbers.reverse();

    char* check_fifo = "/tmp/other_pipe3"; //sending the confirmation
    mkfifo(check_fifo, 0666); 
    int fd2;
    fd2 = open(check_fifo, O_WRONLY); 
    write(fd2, "OK", 3); 
    close(fd2);
    
    printf("Confirmation sent to ipc_rcv\n");


    *shm = '*';
}

void write_message(){
    char num[10];
    int cur_num;
    while (!le_numbers.empty()){
        cur_num = le_numbers.front();
        sprintf(num, "%d", cur_num);
        char* shm = num;
        le_numbers.pop_front();
        printf("Read value on the shared memory: %s\n", shm);
        FILE *f = fopen("numeros.txt", "a");
        fprintf(f, "%s ", shm);
        fclose(f);
    }
}


int main()
{	
    while(1){
        std::thread t1(recieve_message);
        std::thread t2(write_message);
        t1.join();
        t2.join();
        //write_message();
        /*recieve_message();
        write_message();*/
    }
    return 0;
}