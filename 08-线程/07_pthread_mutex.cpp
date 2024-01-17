//线程同步--两个线程数数
#include <iostream>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#define MAX 5000

using namespace std;

int num=0;

//定义互斥锁
pthread_mutex_t mutex;

void *mycount(void *arg)
{
	int *p = (int *)arg;
	for(int i=0; i<MAX; i++)
	{
		//开启互斥锁
		pthread_mutex_lock(&mutex);
		num++;
		cout<<"["<<*p<<"]: "<<num<<endl;
		//关闭互斥锁
		pthread_mutex_unlock(&mutex);
	}
}

int main()
{
	//互斥锁初始化
	pthread_mutex_init(&mutex, NULL);

	pthread_t thread1, thread2;
	int p1=1, p2=2;
	int ret = pthread_create(&thread1, NULL, mycount, &p1);
	if(ret!=0)
	{
		cout<<strerror(ret)<<endl;
		return -1;
	}

	ret = pthread_create(&thread2, NULL, mycount, &p2);
	if(ret!=0)
	{
		cout<<strerror(ret)<<endl;
		return -1;
	}

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	//销毁互斥锁
	pthread_mutex_destroy(&mutex);

	cout<<"main thread, pid="<<getpid()<<" id="<<pthread_self()<<endl;

	return 0;
}
