//读写锁测试程序
#include <iostream>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#define MAX 5000

using namespace std;

int num=0;

//定义互斥锁
pthread_rwlock_t rwlock;

void *mywrite(void *arg)
{
	int *p = (int *)arg;

	while(1)
	{
		//开启写锁
		pthread_rwlock_wrlock(&rwlock);
		num++;
		cout<<"["<<*p<<"]-w: "<<num<<endl;
		//关闭写锁
		pthread_rwlock_unlock(&rwlock);
		usleep(500);
	}
}


void *myread(void *arg)
{
	int *p = (int *)arg;

	while(1)
	{
		//开启读锁
		pthread_rwlock_rdlock(&rwlock);
		cout<<"["<<*p<<"]-r: "<<num<<endl;
		//关闭读锁
		pthread_rwlock_unlock(&rwlock);
		usleep(500);
	}
}

int main()
{
	//互斥锁初始化
	pthread_rwlock_init(&rwlock, NULL);

	pthread_t thread[8];
	int arr[8];

	//创建3个写子线程
	for(int i=0; i<3; i++)
	{
		arr[i]=i;
		int ret = pthread_create(&thread[i], NULL, mywrite, &arr[i]);
		if(ret!=0)
		{
			cout<<strerror(ret)<<endl;
			return -1;
		}
	}

	//创建5个读子线程
	for(int i=3; i<8; i++)
	{
		arr[i]=i;
		int ret = pthread_create(&thread[i], NULL, myread, &arr[i]);
		if(ret!=0)
		{
			cout<<strerror(ret)<<endl;
			return -1;
		}
	}

	//回收子线程
	for(int i=0; i<8; i++)
	{
		pthread_join(thread[i], NULL);
	}

	//销毁互斥锁
	pthread_rwlock_destroy(&rwlock);


	return 0;
}
