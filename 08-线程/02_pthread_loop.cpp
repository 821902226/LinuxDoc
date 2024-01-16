//循环创建子线程，并打印自己是第几个子线程
#include <iostream>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

using namespace std;

void *mythread(void *arg)
{
	int num = *(int *)arg;
	cout<<"我是第"<<num<<"个子线程"<<endl;
	cout<<"child thread, pid="<<getpid()<<" id="<<pthread_self()<<endl;
}

int main()
{
	int i = 0;
	int arr[5];
	pthread_t thread[5];
	for(int i=0; i<5; i++)
	{
		arr[i]=i;
		int ret = pthread_create(&thread[i], NULL, mythread, &arr[i]);	//子线程id，线程属性，子线程处理函数，子线程处理函数的参数
		//cout<<"thread"<<(long)thread<<endl;
		if(ret!=0)
		{
			cout<<strerror(ret)<<endl;
			return -1;
		}
	}
	cout<<"main thread, pid="<<getpid()<<" id="<<pthread_self()<<endl;
	sleep(1);	//让子线程能够执行起来
	//while(1);
	return 0;
}
