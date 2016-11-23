#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

pid_t p1;
pid_t p2;
int pipefd[2];
int sum =0;
void Parent()
{
	kill(p1, SIGUSR1);
	kill(p2, SIGUSR2);
	//kill(p2_pid, SIGUSR2);
}

void func_1()
{
	close(pipefd[0]);
	close(pipefd[1]);
	printf("Child Process 1 is Killed by Parent!\n");
	exit(0);
}

void func_2()
{
	close(pipefd[0]);
	close(pipefd[1]);
	printf("Child Process 2 is Killed by Parent!\n");
	exit(0);
}

int main(void)
{	
	pipe(pipefd);
	int status1, status2;	
	if(signal(SIGINT, Parent) == SIG_ERR)
	{
		printf("can't catch SIGINT.\n");
	}
	p1 = fork();
	if(p1 == 0) //child1
	{
		signal(SIGINT, SIG_IGN);
		if(signal(SIGUSR1, func_1) == SIG_ERR)
		{
			printf("can't catch SIGUSR1.\n");
		}
		sum = 1;
		int count = 0;
		char S1[30] = "Hello World!";
		while(1)
		{
			count++;
			//printf("%d\n", count);
			write(pipefd[1], S1, 20);
			sleep(1);
		}
	}
	else
	{
		p2 = fork();
		if(p2 == 0) //child2
		{
			signal(SIGINT, SIG_IGN);
			if(signal(SIGUSR2, func_2) == SIG_ERR)
			{
				printf("can't catch SIGUSR2.\n");
			}
			char S2[30];
			while(1)
			{
				read(pipefd[0], S2, 20);
				printf("receive from child1:%s\n", S2);
			}
		}
		else
		{
			waitpid(p1, &status1, 0);
			waitpid(p2, &status2, 0);
			close(pipefd[0]);
			close(pipefd[1]);		
			printf("Parent Process is killed!\n");
		}
	}
}
