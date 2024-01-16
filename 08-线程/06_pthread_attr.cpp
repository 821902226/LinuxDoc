//创建子线程时设置分离属性
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
	cout<<"child thread, pid="<<getpid()<<" id="<<pthread_self()<<endl;
}

int main()
{
	//定义pthread_attr_t类型变量
	pthread_attr_t attr;
	//初始化attr变量
	pthread_attr_init(&attr);
	//设置attr为分离属性
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//创建子线程
	int n = 99;
	pthread_t thread;
	int ret = pthread_create(&thread, &attr, mythread, &n);	//子线程id，线程属性，子线程处理函数，子线程处理函数的参数
	cout<<"thread"<<(long)thread<<endl;
	if(ret!=0)
	{
		cout<<strerror(ret)<<endl;
		return -1;
	}
	cout<<"main thread, pid="<<getpid()<<" id="<<pthread_self()<<endl;

	//释放线程属性
	pthread_attr_destroy(&attr);

	
	return 0;
	
}
