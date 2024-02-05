#include <iostream>
#include <string.h>
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <poll.h>
#include <vector>
#include <unordered_map>
#include "wrap.h"	//自定义文件，对socket相关函数进行封装，异常处理

#define MAX 512

using namespace std;

int main()
{
	//创建socket
	int lfd = Socket(AF_INET, SOCK_STREAM, 0);

	//设置端口复用
	int opt_val = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(int));

	//绑定
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(9999);
	serv.sin_addr.s_addr = htonl(INADDR_ANY); //表示使用本机任意可用ip
	Bind(lfd, (struct sockaddr *)&serv, sizeof(serv));

	//监听
	Listen(lfd, 128);

	//poll实现多并发服务器
	struct pollfd client[MAX];
	for(int i=0; i<MAX; i++)
	{
		client[i].fd = -1;
	}
	
	//监听描述符委托给内核监控
	client[0].fd = lfd;
	client[0].events = POLLIN;

	//最大监听范围(表示数组的监听范围)
	int maxfd = 0;

	while(1)
	{
		int flag = poll(client, maxfd+1, -1); // -1 表示永久阻塞
		if(flag==-1)
		{
			if(errno==EINTR)	//被信号中断
			{
				continue;
			}
			perror("poll error");
			break;
		}

		//有客户端请求连接
		if(client[0].revents & POLLIN) //revents可能包含不止一个事件
		{
			int cfd = Accept(lfd, NULL, NULL);
			
			//将cfd添加到clients集合中, 需要找到空闲位置
			int i;
			for(i=1; i<MAX; i++)
			{
				if(client[i].fd==-1)
				{
					client[i].fd = cfd;
					client[i].events = POLLIN;
					break;
				}
			} 
			//没有可用位置，直接关闭cfd
			if(i==MAX)
			{
				close(cfd);
				continue;
			}

			//修改内核监控范围
			if(maxfd<i)
			{
				maxfd = i;
			}

			if(--flag==0)
			{
				continue;
			}
		}

		int n = 0;
		char buf[MAX];
		//有数据发来的情况
		for(int i=0; i<=maxfd; i++)
		{
			int sockfd = client[i].fd;

			if(sockfd==-1)
			{
				continue;
			}

			if(client[i].revents & POLLIN)
			{
				memset(buf, 0, sizeof(buf));
				n = Read(sockfd, buf, sizeof(buf));

				if(n<=0)
				{
					//关闭连接
					close(sockfd);
					//将sockfd从client中删除
					client[i].fd=-1;
				}
				else
				{
					cout<<"read data: "<<buf<<endl;
					for(int i=0; i<n; i++)
					{
						buf[i] = toupper(buf[i]);
					}
	
					Write(sockfd, buf, n);
				}
			}
		}
	}
	
	//关闭监听描述符
	close(lfd);

	return 0;
}
