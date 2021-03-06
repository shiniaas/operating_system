/*#include <pthread.h>
#include <sys/types.h>
#include <linux/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <error.h>
#include <signal.h>

#define MYKEY 6666
#define SHARED_MEMORY_SIZE 10
char text[SHARED_MEMORY_SIZE];
int semid;
//0:in 1:out 2:mutex
int a = 0;
FILE* fr, *fw;
int position_in = 0;
int position_out = 0;
char* shm = NULL;
int shmid;

void P(int semid, int index)
{
	struct sembuf sem;
	sem.sem_num = index;
	sem.sem_op = -1;
	sem.sem_flg = 0;
	semop(semid, &sem, 1);
	return;
}

void V(int semid, int index)
{
	struct sembuf sem;
	sem.sem_num = index;
	sem.sem_op = 1;
	sem.sem_flg = 0;
	semop(semid, &sem, 1);
	return;
}

void func_1()
{
	while(semctl(semid, 1, GETVAL, 0) != 0) 
	{
		//P(semid, 2);
		P(semid, 1);
		fprintf(fw, "%c", shm[position_out]);
		position_out = (position_out+1)%SHARED_MEMORY_SIZE;
		//V(semid, 2);
	}
	exit(0);
}

int main(void)
{
	char ** argv = NULL;
	shmid = shmget((key_t)1234, sizeof(text), 0666|IPC_CREAT);
	if(shmid == -1)  
    {  
        printf("shmget failed\n");  
        exit(-1);  
    }
	shm = (char*)shmat(shmid,NULL,SHM_R|SHM_W);
	shm[0] = 0;
	semid = semget(MYKEY, 3, IPC_CREAT|0666);
	if(semid == -1)
	{
		printf("semget error");
		exit(-1);
	}	
	semctl(semid, 0, SETVAL, SHARED_MEMORY_SIZE);
	semctl(semid, 1, SETVAL, 0);
	semctl(semid, 2, SETVAL, 1);
	pid_t p1, p2;
	fr = fopen("read.txt", "r");
	if(fr == NULL)
	{
		printf("open read.txt error\n");
		exit(-1);
	}
	fw = fopen("write.txt", "w");
	if(fw == NULL)
	{
		printf("open write.txt error");
		exit(-1);
	}
	if((p1=fork()) == 0)	//readbuf
	{
		if(signal(SIGUSR1, func_1) == SIG_ERR)
		{
			printf("can't catch SIGUSR1.\n");
		}
		while(1)
		{
			P(semid, 1);
			P(semid, 2);
			fprintf(fw, "%c", shm[position_out]);
			position_out = (position_out+1)%SHARED_MEMORY_SIZE;
			V(semid, 2);
			V(semid, 0);
		}	
	}
	else
	{
		if((p2=fork()) == 0)	//writebuf
		{
			while(1)
			{	
				P(semid, 0);
				P(semid, 2);
				if(fscanf(fr, "%c", &shm[position_in])!=1)
				{
					kill(p1, SIGUSR1);
					break;
				}
				position_in = (position_in+1)%SHARED_MEMORY_SIZE;
				V(semid, 2);		
				V(semid, 1);
			}
		}
		else	//main
		{
			wait(NULL);
			wait(NULL);
			semctl(semid, 0, IPC_RMID, 0); //delete one is true
			shmctl(shmid, IPC_RMID, 0);
			fclose(fr);
			fclose(fw);
		}
	}
	
	return 0;
}


