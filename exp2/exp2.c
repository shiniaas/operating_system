#include <pthread.h>
#include <sys/types.h>
#include <linux/sem.h>
#include <stdio.h>
#include <stdlib.h>

#define MYKEY 6666

int semid;
//0:print 1:add 2:mutex
int a = 0;

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

void *subp1()
{
	int i = 1;
	for(i; i <= 100; i++)
	{
		P(semid, 0);
		//P(semid, 3);
		printf("%d\n", a);
		//V(semid, 3);
		V(semid, 1);
	}
	return;
}

void *subp2()
{
	int i = 1;
	for(i; i <= 100; i++)
	{
		P(semid, 1);
		//P(semid, 3);
		a = a+i;
		//V(semid, 3);
		V(semid, 0);
	}
}

int main(void)
{
	semid = semget(MYKEY, 2, IPC_CREAT|0666);
	printf("%d\n", semid);
	if(semid == -1)
	{
		printf("semget error");
		exit(-1);
	}	
	pthread_t p1, p2;
	semctl(semid, 0, SETVAL, 0);
	semctl(semid, 1, SETVAL, 1);
	//semctl(semid, 3, SETVAL, 1);
	pthread_create(&p1, NULL, subp1, NULL);
	pthread_create(&p2, NULL, subp2, NULL);
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	semctl(semid, 0, IPC_RMID, 0); //delete one is true
	return 0;
}


