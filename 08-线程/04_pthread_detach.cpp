//设置子线程为分离属性，能够自己回收资源
#include <iostream>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

using namespace std;

void *mythread(void *arg)
{
	int num = *(int *)arg;
	cout<<"接收到的参数："<<num<<endl;
	cout<<"hello world"<<endl;
	cout<<"child thread, pid="<<getpid()<<" id="<<pthread_self()<<endl;
}

int main()
{
	int n = 99;
	pthread_t thread;
	int ret = pthread_create(&thread, NULL, mythread, &n);	//子线程id，线程属性，子线程处理函数，子线程处理函数的参数
	cout<<"thread"<<(long)thread<<endl;
	if(ret!=0)
	{
		cout<<strerror(ret)<<endl;
		return -1;
	}
	cout<<"main thread, pid="<<getpid()<<" id="<<pthread_self()<<endl;
	//设置子线程分离属性
	pthread_detach(thread); //参数标识哪个子线程

	sleep(1);	//让子线程能够执行起来
	//while(1);
	return 0;
}
