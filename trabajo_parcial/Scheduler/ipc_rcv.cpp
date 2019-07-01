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
#include <list>
#include <thread>

#define MAXSIZE 64

#define MAX_MSG_SIZE 40

#define SHMSZ 27
#define LE_KEY 5678
#define IPC_KEY 1234

std::list<int> le_numbers; //queue ordered by max

//g++ ipc_rcv.cpp -o ipc_rcv -std=c++11 -lpthread


void die(char *s)
{
  perror(s);
  exit(1);
}

struct msg_buf
{
    long    mtype;
    char    mtext[MAXSIZE];
};

struct msg_buf rcvbuffer; //global variable

bool check(){
    int fd;
    char * myfifo = "/tmp/other_pipe3"; //confirmation from shm_client
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

void send_message(){
  /////////SHARED MEMORY////////
  while (!le_numbers.empty()){
    char c;
    char num[10];
    int shmid;
    key_t key;
    char *shm, *s; //shm
  
  ////////////////////////////////////// 

  //shared memory (based on: shm_server.c)  
    //We'll name our shared memory segment "5678".
    key = LE_KEY;
    //create the segment
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
      perror("shmget");
      exit(1);
    } 

    
  //Now we attach the segment to our data space.
    if ((shm = (char*) shmat(shmid, NULL, 0)) == (char *) (-1)) {
      perror("shmat");
      exit(1);
    }

    *shm = '*'; //so shm waits for a value

  //looping
    s = shm;

    int front = le_numbers.front();
            
    //char* rbuf = rcvbuffer.mtext;
    sprintf(num, "%d", front);            
    char* rbuf = num;
    le_numbers.pop_front();
    for (rbuf; *rbuf != '\0'; rbuf++){
      *s = *rbuf;
      s++;            
    }
    *s = NULL;            

    if (!check()){
      printf("Confirmation is wrong\n");
      exit(0);
    }
            
    while (*shm != '*'){
          //printf("Waiting for the shared memory to be read\n");        
            //sleep(1);
    }
  }
}

void recieve_message(){
  int msqid;
  key_t key;
  //struct msg_buf rcvbuffer;

  key = IPC_KEY;

  while ((msqid = msgget(key, 0666)) < 0){
         //die("msgget()");
        printf("Waiting for IPC Creation\n");
        //sleep(1);
  }


     //Receive an answer of message type 1.
  if (msgrcv(msqid, &rcvbuffer, MAXSIZE, 1, 0) < 0)
    die("msgrcv");
    
  printf("message recieved: %s\n", rcvbuffer.mtext);
  int num = atoi(rcvbuffer.mtext);
  le_numbers.push_back(num);
  le_numbers.sort();

  //"OK" via pipe
  char* check_fifo = "/tmp/other_pipe2"; //sending the confirmation to pipe_read
  mkfifo(check_fifo, 0666); 
  int fd2;
  fd2 = open(check_fifo, O_WRONLY);
  write(fd2, "OK", 3); 
  close(fd2);

  printf("Sending confirmation to pipe_read\n");
}

int main()
{
  while(1){
    std::thread t1(recieve_message);
    std::thread t2(send_message);
    t1.join();
    t2.join();
    //send_message();
  }  
    return 0;
}
