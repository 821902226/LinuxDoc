//取消线程和设置取消点
#include <iostream>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

using namespace std;

void *mythread(void *arg)
{
	while(true)
	{	
		cout<<"hello world"<<endl;
		//pthread_testcancel(); //手动设置取消点
	}
}

int main()
{
	int n = 99;
	pthread_t thread;
	int ret = pthread_create(&thread, NULL, mythread, &n);	//子线程id，线程属性，子线程处理函数，子线程处理函数的参数
	if(ret!=0)
	{
		cout<<strerror(ret)<<endl;
		return -1;
	}
	cout<<"main thread, pid="<<getpid()<<" id="<<pthread_self()<<endl;
	sleep(1);
	//取消线程（要有取消点，系统调用或者是pthread_testcancel函数手动设置）
	pthread_cancel(thread);

	pthread_join(thread, NULL);

	return 0;
}
