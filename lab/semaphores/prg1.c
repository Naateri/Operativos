#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define SHM_KEY 0x12345
#define SEM_KEY 0x55342
#define MAX_TRIES 20

struct shmseg {
   int write_complete;
   int read_complete;
	int v1, v2, v3;
	int val_changed;
};

int pares, numtimes;

void shared_memory_cntr_increment(int, struct shmseg*, int);

void remove_semaphore();

int main(int argc, char *argv[]) {
   int shmid;
   struct shmseg *shmp;
   char *bufptr;
   int total_count;
   int sleep_time;
   pid_t pid; 
	char* type; type = argv[1];
	long RandNum = 0;
	srand(time(NULL));
	RandNum = rand()% 100;
	printf("Type: %s\n", type);
	if (strcmp(type, "pares") == 0){
		if (RandNum & 1) RandNum <<= 1;
		pares = 1;
		numtimes = 0;
	} else if (strcmp(type, "impares") == 0){
		if (!(RandNum & 1)) RandNum--;
		pares = 0;
		numtimes = 1;
	} else {
		printf("Error\n");
		return 0;
	}	 

	//printf("Number is: %d\n", RandNum);
   shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
   
   if (shmid == -1) {
      perror("Shared memory");
      return 1;
   }
   // Attach to the segment to get a pointer to it.
   shmp = shmat(shmid, NULL, 0);
   
   if (shmp == (void *) -1) {
      perror("Shared memory attach: ");
      return 1;
   }
   shmp->val_changed = 0;
   while(numtimes <= 10000){
   		shared_memory_cntr_increment(pid, shmp, RandNum);
   		//remove_semaphore();
   	}
   while (shmp->read_complete != 1){   	
   		sleep(1);
	}
   
   if (shmdt(shmp) == -1) {
      perror("shmdt");
      return 1;
   }
   
   if (shmctl(shmid, IPC_RMID, 0) == -1) {
      perror("shmctl");
      return 1;
   }
   printf("Writing Process: Complete\n");
   remove_semaphore();
   return 0;
}

void shared_memory_cntr_increment(int pid, struct shmseg *shmp, int le_num) {   
   int semid;
   struct sembuf sem_buf;
   struct semid_ds buf;
   int retval;
   semid = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0666);   
   
   /* Got the semaphore */
   if (semid >= 0) {
      printf("First Process\n");
      sem_buf.sem_op = 1;
      sem_buf.sem_flg = 0;
      sem_buf.sem_num = 0;
      retval = semop(semid, &sem_buf, 1);
      if (retval == -1) {
         perror("Semaphore Operation: ");
         return;
      }      

   } else if (errno == EEXIST) { // Already other process got it
      int ready = 0;
      printf("Second Process\n");
      semid = semget(SEM_KEY, 1, 0);
      if (semid < 0) {
         perror("Semaphore GET: ");
         return;
      }
      printf("Waiting...\n");
      /* Waiting for the resource */
      sem_buf.sem_num = 0;
      sem_buf.sem_op = 0;
      sem_buf.sem_flg = SEM_UNDO;
      retval = semop(semid, &sem_buf, 1);
      if (retval == -1) {
         perror("Semaphore Locked: ");
         return;
      }
   }
   sem_buf.sem_num = 0;
   sem_buf.sem_op = -1; /* Allocating the resources */
   sem_buf.sem_flg = SEM_UNDO;
   retval = semop(semid, &sem_buf, 1);
   
   if (retval == -1) {
      perror("Semaphore Locked: ");
      return;
   }
   shmp->write_complete = 0;
   printf("SHM_WRITE: Now writing\n");
   //Seccion critica
   //srand(time(NULL));
   /*for (numtimes; numtimes < 20; numtimes+=2) {
   		printf("Number passed: %d\n", numtimes);    
		shmp->v1 = shmp->v2 = shmp->v3 = numtimes;
		shmp->val_changed = 1;
		//le_num = rand()%50; */

		/*if (pares == 1 && le_num & 1) le_num <<= 1;
		else if (pares == 0 && ! (le_num & 1)) le_num--;*/		
      /*while (shmp->val_changed != 0){
      	printf("Sleeping\n");
      	sleep(1);      	      	

      } 
   }*/

	printf("Number passed: %d\n", numtimes);
	shmp->v1 = shmp->v2 = shmp->v3 = numtimes;
	shmp->val_changed = 1;
	numtimes += 2;
	//sleep(1);

   shmp->write_complete = 1;
   sem_buf.sem_op = 1; /* Releasing the resource */
   retval = semop(semid, &sem_buf, 1);
   
   if (retval == -1) {
      perror("Semaphore Locked\n");
      return;
   }
	printf("SHM_WRITE: Writing Done\n");
	//remove_semaphore();
	return;
}
   
void remove_semaphore() {
   int semid;
   int retval;
   semid = semget(SEM_KEY, 1, 0);
      if (semid < 0) {
         perror("Remove Semaphore: Semaphore GET: ");
         return;
      }
   retval = semctl(semid, 0, IPC_RMID);
   if (retval == -1) {
      perror("Remove Semaphore: Semaphore CTL: ");
      return;
   }
   return;
}
