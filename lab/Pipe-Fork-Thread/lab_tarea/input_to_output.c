#include  <fcntl.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <sys/types.h>
#include  <sys/wait.h>
#include  <sys/stat.h>
#include  <termios.h>
#include  <unistd.h>

int main(int argc, char* argv[])
{


pid_t pid;
int ofd, ifd;
int fd[2];
char *cmd,  *cmd2, *params;
cmd = argv[1];
cmd2 = argv[2];
params = argv[3];
pipe(fd);
pid = fork();

if(pid==0) // Child
   {
    printf("Running %s | %s\n",cmd,cmd2);
    dup2(fd[0],0);
    close(fd[0]);
    execlp(cmd2,cmd2,params, (char*) NULL); 
    }
else {
  dup2(fd[1],1);
  close(fd[1]);
  execlp(cmd,cmd,(char*) NULL);
}
wait((int *)NULL);
}
