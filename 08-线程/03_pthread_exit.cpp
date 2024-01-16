//线程退出函数pthread_exit 主线程等待子线程退出pthread_join
#include <iostream>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

using namespace std;

void *mythread(void *arg)
{
	int *num = (int *)arg;
	cout<<"child thread, pid="<<getpid()<<" id="<<pthread_self()<<endl;
	pthread_exit(num); //退出子线程
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
	//回收子线程
	void * p;
	pthread_join(thread, &p);
	cout<<"child exit status: "<<*(int*)p<<endl;
	return 0;
}
