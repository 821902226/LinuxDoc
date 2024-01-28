#include <iostream>
#include <string.h>
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

//子线程回调函数
void *thread_work(void *fid)
{
	int cfd = *(int *)fid;

	char buf[1024];
	while(1)
	{
		memset(buf, 0, sizeof(buf));
		int size = Read(cfd, buf, sizeof(buf));

		if(size<=0)
		{
			cout<<"read data error"<<endl;
			break;
		}
			
		cout<<"read data: "<<buf<<endl;

		//将接收到的数据转换成大写
		for(int i=0; i<size; i++)
		{
			buf[i] = toupper(buf[i]);
		}

		//将接收到的数据发送回客户端
		write(cfd, buf, size);
		
		//关闭cfd
		close(cfd);
		pthread_exit(NULL);
	}
}

int main()
{
	//创建socket
	int lfd = Socket(AF_INET, SOCK_STREAM, 0);

	//绑定
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(9999);
	serv.sin_addr.s_addr = htonl(INADDR_ANY); //表示使用本机任意可用ip
	Bind(lfd, (struct sockaddr *)&serv, sizeof(serv));

	//监听
	Listen(lfd, 128);

	struct sockaddr_in client;
	socklen_t len;
	char ip[16];
	int idx = 0;
	unordered_map<int, int> map;
	while(1)
	{
		//接受连接
		len = sizeof(client);
		int cfd = Accept(lfd, (struct sockaddr *)&client, &len);
		map.insert(make_pair(idx, cfd));
		cout<<"client ip: "<<inet_ntop(AF_INET, &client.sin_addr.s_addr, ip, sizeof(ip))<<" port: "<<ntohs(client.sin_port)<<endl;

		//创建子线程
		pthread_t pid = pthread_create(&pid, NULL, thread_work, &map[idx]);
		//设置子线程分离属性
		pthread_detach(pid);

		idx++;
	}
	
	//关闭监听描述符
	close(lfd);

	return 0;
}
