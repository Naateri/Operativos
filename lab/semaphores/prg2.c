#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>

#define SHM_KEY 0x12345
struct shmseg {
   int write_complete;
   int read_complete;
   int v1, v2, v3;
   int val_changed;
};

int main(int argc, char *argv[]) {
   int shmid, numtimes;
   struct shmseg *shmp;
   int total_count;
   int cntr;
   int sleep_time;
   int prev_num = -1;
   
   shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
   
   if (shmid == -1) {
      perror("Shared memory");
      return 1;
   }
   // Attach to the segment to get a pointer to it.
   while(1){
   shmp = shmat(shmid, NULL, 0);
   
   if (shmp == (void *) -1) {
      /*perror("Shared memory attach");
      return 1;*/
      sleep(1);
      shmp = shmat(shmid, NULL, 0);   
   }
   //read shm
   while (shmp->write_complete != 1) {
      if (shmp->v1 == -1) {
         perror("read");
         return 1;
      }
      if (shmp->val_changed == 1){
         printf("Number: %d, %d, %d\n", shmp->v1, shmp->v2, shmp->v3);
         shmp->val_changed = 0;
      } 
      //sleep(1);
   }
   
   shmp->read_complete = 1;
   
   if (shmdt(shmp) == -1) {
      perror("shmdt");
      return 1;
   }
   } //end while
   return 0;
}
