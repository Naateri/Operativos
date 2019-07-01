#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stack>
#include <thread>
#include <ctime>

//g++ pipe_read.cpp -o pipe_read -std=c++11 -lpthread -fno-stack-protector

std::stack<int> le_numbers;
char read_buffer[80]; 

#define MAX_MSG_SIZE 40

#define SHMSZ 27

#define MAXSIZE 64 //IPC 

struct msg_buf //ipc
{
    long    mtype;
    char    mtext[MAXSIZE];
};

//struct msg_buf sbuf; //IPC

void die(char *s) //IPC
{
  perror(s);
  exit(1);
}

void send_message();

pid_t childpid;

void reading_pipe(){
	//shared memory, now ipc

    int fd;
    char * myfifo = "/tmp/myfifo";       /*codigo de para leer de pipeWrite*/
    char buf[MAX_MSG_SIZE];
    fd = open(myfifo,O_RDONLY);
    /////////////////////////////////////
    int pidPadre=getpid();
    int nbytes;
    int pipePaH[2];    
    pid_t childpid;
    char readbuffer[80]; 
    int childID = 0;
    pipe(pipePaH);                        /*codigo del fork*/
	char out_buffer[MAX_MSG_SIZE];
	char out_buffer2[MAX_MSG_SIZE];
	char* num;
	
	//IPC
	
    if((childpid = fork())== -1)
    {
        perror("fork");
        exit(1);
    }
    //while (1) { 
        if(childpid == (pid_t) 0)
        {
            close(pipePaH[0]);
           ////////////////////////////////////////////
            //Lee lo que escribe pipeWrite
            while (read(fd, buf, 80) == -1){
				//sleep(1);
				//printf("Empty pipe\n");
			}        

			read(fd,buf,80);
            //printf("Received pipeWrite: %s\n",buf);
            //////////////////////////////////////////////////////////////////////

            //dup2(pipePaH[0], 0);
            write(pipePaH[1],buf,(strlen(buf)+1)); /*escribe el padre en el pipe*/
                        
            //////////////////////////////////////////////////////////////////////
                         
            //bzero(out_buffer,strlen(out_buffer));
            //printf ("Response sent to parent.\n");        

            //open(pipePaH[0], O_RDONLY);
            kill(getpid(), SIGKILL); //so no more childs are created whilst looping
       }
        else
        {	
            //dup2(pipePaH[1], 1);
            close(pipePaH[1]);
            //bzero(readbuffer,80);
            //nbytes = read(pipePaH[0],readbuffer,sizeof(readbuffer));
            //read(pipePaH[0],readbuffer,sizeof(readbuffer));
            read(pipePaH[0],readbuffer,80);
            //printf("Child read: %s\n",readbuffer);
            printf("Parent read: %s\n",readbuffer);

            char* num = readbuffer;
            int true_num = atoi(num);

            //printf("Pushing to stack\n");
            le_numbers.push(true_num);
            //le_numbers.push(readbuffer);

            char* check_fifo = "/tmp/other_pipe"; //sending the confirmation to pipe_write
            mkfifo(check_fifo, 0777); 
            int fd2;
            fd2 = open(check_fifo, O_WRONLY); 
            write(fd2, "OK", 3); 
            close(fd2);
            
            printf("Sending confirmation to pipe_write\n");

            close(fd);
    		//kill(getpid(), SIGKILL); //so no more childs are created whilst looping
    	}
    //printf("Stack size: %d\n", le_numbers.size());
    //unlink(myfifo);
    //close(fd);
}

bool check(){
    int fd;
    char * myfifo = "/tmp/other_pipe2"; //confirmation from ipc_rcv
    char buf[80];
    fd = open(myfifo,O_RDONLY);
    clock_t begin = clock();
    while (read(fd, buf, 80) == -1){
        /*clock_t end = clock();
        if (double(end - begin) >= 2) return 1;*/
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

    //printf("Stack size: %d\n", le_numbers.size());

    if (!le_numbers.empty()){
        struct msg_buf sbuf; //IPC
        int msqid;
        int msgflg = IPC_CREAT | 0666;
        key_t key;
        size_t buflen;
        key = 1234;                      

        if ((msqid = msgget(key, msgflg)) < 0)   //Get the message queue ID for the given key
            die("msgget");
            
        //Message Type
        sbuf.mtype = 1;

        buflen = strlen(sbuf.mtext) + 1;
        int cur_num = le_numbers.top();
        char temp[10];
        //printf("Message from stack: %s", temp);
        le_numbers.pop();

        sprintf(temp, "%d", cur_num);

        int i = 0;
        char* rbuf = temp;
        //char* rbuf = readbuffer;
            
        for (rbuf; *rbuf != NULL; rbuf++){
            putchar(*rbuf);
            sbuf.mtext[i++] = *rbuf;
        }
        putchar('\n');
        sbuf.mtext[i++] = '\0';
        if (msgsnd(msqid, &sbuf, buflen, IPC_NOWAIT) < 0){
            printf ("%d, %ld, %s, %d \n", msqid, sbuf.mtype, sbuf.mtext, (int)buflen);
            die("msgsnd");
        } else {
            if (!check()){
                printf("Confirmation is wrong\n");
                exit(0);
            }
            printf("Confirmation with ipc_rcv done\n");
            printf("Message %s Sent\n", sbuf.mtext);
        }
    }
}

int main() { 		
	/*if((childpid = fork())== -1)
    {
        perror("fork");
        exit(1);
    }*/
	while(1){
        /*reading_pipe();
        send_message();*/
        std::thread t1(reading_pipe);
        std::thread t2(send_message);
        t1.join();
        t2.join();
        //send_message();
    }

    return 0; 
} 
