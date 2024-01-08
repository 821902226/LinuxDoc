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
		write(fd[1], "hello world", sizeof("hello world"));	
		wait(NULL);
	}
	else
	{
		//关闭子进程的写端
		close(fd[1]);
		char str[4096];
		memset(str, 0, sizeof(str));
		read(fd[0], str, sizeof(str));
		cout<<"读取到的数据: "<<str<<endl;
	}
	return 0;	
}
