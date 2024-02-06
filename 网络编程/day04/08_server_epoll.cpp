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
#include <sys/epoll.h>
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
	serv.sin_port = htons(8888);
	serv.sin_addr.s_addr = htonl(INADDR_ANY); //表示使用本机任意可用ip
	Bind(lfd, (struct sockaddr *)&serv, sizeof(serv));

	//监听
	Listen(lfd, 128);

	//创建一颗eopll树
	int epfd = epoll_create(1);
	if(epfd==-1)
	{
		perror("Create epoll error!");
		exit(-1);
	}

	//将监听描述符上树
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = lfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
	
	struct epoll_event evs[1024];
	while(1)
	{
		memset(evs, 0, sizeof(evs));
		int flag = epoll_wait(epfd, evs, 1024, -1); // -1 表示永久阻塞,直到有事件发生
		if(flag==-1)
		{
			if(errno==EINTR)	//被信号中断
			{
				continue;
			}
			perror("epoll error");
			break;
		}

		int n;
		char buf[1024];
		//遍历所有发生的事件
		for(int i=0; i<flag; i++)
		{
			int sockfd = evs[i].data.fd;
			if(sockfd==lfd)
			{
				int cfd = Accept(lfd, NULL, NULL);
				ev.events = EPOLLIN;
				ev.data.fd = cfd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
			}
			else
			{
				memset(buf, 0, sizeof(buf));
				n = Read(sockfd, buf, sizeof(buf));

				if(n<=0)
				{
					//关闭连接
					close(sockfd);
					//将sockfd从epoll树中删除
					epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
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
	
	//关闭树根描述符
	close(epfd);
	//关闭监听描述符
	close(lfd);

	return 0;
}
