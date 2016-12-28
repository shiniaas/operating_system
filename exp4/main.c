#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>

void printdir(char* dir, int depth)
{
	DIR* dp;
	struct dirent *entry;
	struct stat statbuf;	
	int size;
	if((dp = opendir(dir)) == NULL)
	{
		printf("open %s error\n", dir);
		return;
	}
	chdir(dir);
	while((entry = readdir(dp)) != NULL)
	{
		lstat(entry->d_name, &statbuf);
		if(S_ISDIR(statbuf.st_mode))
		{
			if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..") == 0)
			{
				continue;
			}
			size = statbuf.st_size;
			printf("depth:%d dir:%s size:%d\n", depth, entry->d_name, size);
			printdir(entry->d_name, depth+4);
		}
		else
		{
			size = statbuf.st_size;
			printf("depth:%d file:%s size:%d\n", depth, entry->d_name, size);
		}
	}
	chdir("..");
	closedir(dp);
}

int main(int argc, char* argv[])
{
	printdir("/home/lifan/Desktop/test", 0);
	return 0;
}

