//线程同步--条件变量
#include <iostream>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

using namespace std;

//链表结构体
typedef struct node
{
	int data;
	struct node *next;
}Node;

Node *head=NULL;

//定义互斥锁
pthread_mutex_t mutex;
//定义条件变量
pthread_cond_t cond;

void *producer(void *arg)
{
	Node *pnode = NULL;
	while(1)
	{
		//生产一个节点
		pnode = new Node;
		if(pnode==NULL)
		{
			perror("new error!");
			exit(-1);
		}
		pnode->data = rand()%100;

		//加锁
		pthread_mutex_lock(&mutex);
		//头插法
		pnode->next = head;
		head = pnode;
		cout<<"producer: "<<head->data<<endl;
		//解锁
		pthread_mutex_unlock(&mutex);
		
		//通知消费者线程解除阻塞
		pthread_cond_signal(&cond);

		usleep(30000);
	}
}

void *consumer(void *arg)
{
	Node *pnode = NULL;
	while(1)
	{
		pthread_mutex_lock(&mutex);

		if(head==NULL)
		{
			//若条件不满足，需要等待阻塞
			//条件不满足，则阻塞等待并解锁
			//条件满足则解除阻塞并加锁
			pthread_cond_wait(&cond, &mutex);
		}

		cout<<"consumer: "<<head->data<<endl;
		pnode = head;
		head = head->next;
		pthread_mutex_unlock(&mutex);
		delete pnode;
		pnode=NULL;

		usleep(30000);
	}

}

int main()
{
	//互斥锁初始化
	pthread_mutex_init(&mutex, NULL);
	//条件变量初始化
	pthread_cond_init(&cond, NULL);

	pthread_t thread1, thread2;
	int ret = pthread_create(&thread1, NULL, producer, NULL);
	if(ret!=0)
	{
		cout<<strerror(ret)<<endl;
		return -1;
	}

	ret = pthread_create(&thread2, NULL, consumer, NULL);
	if(ret!=0)
	{
		cout<<strerror(ret)<<endl;
		return -1;
	}

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	//销毁互斥锁
	pthread_mutex_destroy(&mutex);
	//销毁条件变量
	pthread_cond_destroy(&cond);

	return 0;
}
