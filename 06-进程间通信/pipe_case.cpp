//使用pipe完成ps -aux | grep bash 命令
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

using namespace std;

int main()
{
	//创建一个管道
	int fd[2];
	int res = pipe(fd);
	if(res<0)
	{
		perror("create pipe error");
		return -1;
	}

	//创建子进程
	pid_t pid = fork();
	if(pid<0)
	{
		//创建失败
		perror("create process error");
		return -1;
	}
	else if(pid>0)
	{
		//关闭父进程的读端
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		execl("/bin/ps", "ps", "aux", NULL);
		wait(NULL);
	}
	else
	{
		//关闭子进程的写端
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		execl("/bin/grep", "grep", "bash", NULL);
	}
	return 0;	
}
