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


#define MAX_MSG_SIZE 40

#define SHMSZ 27

#define MAXSIZE 64 //IPC 

struct msgbuf //ipc
{
    long    mtype;
    char    mtext[MAXSIZE];
};

void die(char *s) //IPC
{
  perror(s);
  exit(1);
}

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
    
    /*
     * Now put some things into the memory for the
     * other process to read.
     */    
        
    //shared memory //

    ////////////////////////////////////   
    if((childpid = fork())== -1)
    {
        perror("fork");
        exit(1);
    }
    //while (1) { 
        if(childpid != 0)
        {
           ////////////////////////////////////////////
            //Lee lo que escribe pipeWrite
            while (read(fd, buf, 80) == -1){
                sleep(1);
                //printf("Empty pipe\n");
            }        

            read(fd,buf,80);
            printf("ODD NUMBER\n");
            printf("Received pipeWrite: %s\n",buf);
            //////////////////////////////////////////////////////////////////////
            
            //dup2(pipePaH[0], 0);
            close(pipePaH[0]);
            write(pipePaH[1],buf,(strlen(buf)+1)); /*escribe el padre en el pipe*/
                        
            //////////////////////////////////////////////////////////////////////
                         
            //bzero(out_buffer,strlen(out_buffer));
            printf ("Response sent to child.\n");        

            //open(pipePaH[0], O_RDONLY);

       }
        else
        {   
            //dup2(pipePaH[1], 1);
            close(pipePaH[1]);
            //bzero(readbuffer,80);
            //nbytes = read(pipePaH[0],readbuffer,sizeof(readbuffer));
            //read(pipePaH[0],readbuffer,sizeof(readbuffer));
            read(pipePaH[0],readbuffer,80);
            printf("Child read: %s\n",readbuffer);                     
            //close(pipePaH[0]);            

            struct msgbuf sbuf; //IPC
            int msqid;
            int msgflg = IPC_CREAT | 0666;
            key_t key;    
            size_t buflen;
            key = 1092;                      

            if ((msqid = msgget(key, msgflg )) < 0)   //Get the message queue ID for the given key
                die("msgget");
            
            //Message Type
            sbuf.mtype = 1;    

            buflen = strlen(sbuf.mtext) + 1 ;

            int i = 0;
            char* rbuf = readbuffer;            
            
            for (rbuf; *rbuf != NULL; rbuf++){
                putchar(*rbuf);
                sbuf.mtext[i++] = *rbuf;
            }
            putchar('\n');
            sbuf.mtext[i++] = '\0';

            if (msgsnd(msqid, &sbuf, buflen, IPC_NOWAIT) < 0)
            {
                printf ("%d, %ld, %s, %d \n", msqid, sbuf.mtype, sbuf.mtext, (int)buflen);
                die("msgsnd");
            }

            else {
                printf("Message %s Sent\n", sbuf.mtext);
            }
            
            
            //open(pipePaH[1], O_RDONLY);

            close(fd);
            kill(getpid(), SIGKILL); //so no more childs are created whilst looping
        }

    //unlink(myfifo);
    //close(fd);
}

int main() {        
    /*if((childpid = fork())== -1)
    {
        perror("fork");
        exit(1);
    }*/
    while(1) reading_pipe();

    return 0; 
} 
