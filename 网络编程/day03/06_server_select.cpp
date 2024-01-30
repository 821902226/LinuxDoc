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
#include <unordered_map>
#include "wrap.h"	//自定义文件，对socket相关函数进行封装，异常处理

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

	//定义fd_set类型的变量
	fd_set readfdset, tempfdset;
	//清空readfdset集合
	FD_ZERO(&readfdset);
	FD_ZERO(&tempfdset);
	//将lfd加入到readfdset中，委托内核监控
	FD_SET(lfd, &readfdset);
	//最大监听范围
	int maxfd = lfd;
	
	//存储所有连接的客户端
	vector<int> clients;

	while(1)
	{
		tempfdset = readfdset;
		int flag = select(maxfd+1, &tempfdset, NULL, NULL, NULL);
		if(flag==-1)
		{
			if(errno==EINTR)	//被信号中断
			{
				continue;
			}
			break;
		}
		//有客户端请求连接
		if(FD_ISSET(lfd, &tempfdset))
		{
			int cfd = Accept(lfd, NULL, NULL);
			
			//将cfd添加到clients集合中, 并将cfd加入监视集合tempfdset--用于判断cfd是否就绪
			FD_SET(cfd, &tempfdset);
			clients.push_back(cfd);

			//修改内核监控范围
			if(maxfd<cfd)
			{
				maxfd = cfd;
			}
		}

		int n = 0;
		char buf[1024];
		//有数据发来的情况
		for(auto it = clients.begin(); it!=clients.end(); it++)
		{
			int client = *it;
			if(FD_ISSET(client, &tempfdset))	//判断套接字cfd是否就绪
			{
				memset(buf, 0, sizeof(buf));
				n = Read(client, buf, sizeof(buf));

				if(n<=0)
				{
					//关闭连接
					close(client);
					//将i从readfdset中删除
					clients.erase(it);
				}
				else
				{
					cout<<"read data: "<<buf<<endl;
					for(int i=0; i<n; i++)
					{
						buf[i] = toupper(buf[i]);
					}

					Write(client, buf, n);
				}
			}
		}
	}
	
	//关闭监听描述符
	close(lfd);

	return 0;
}
