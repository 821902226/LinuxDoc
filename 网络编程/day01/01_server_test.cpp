#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>

using namespace std;

int main()
{
	//创建socket
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if(lfd<0)
	{
		perror("socket error");
		return -1;
	}

	//绑定
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(9999);
	serv.sin_addr.s_addr = htonl(INADDR_ANY); //表示使用本机任意可用ip
	int ret = bind(lfd, (struct sockaddr *)&serv, sizeof(serv));
	if(ret<0)
	{
		perror("bind error");
		return -1;
	}

	//监听
	listen(lfd, 128);

	//接受连接
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	int cfd = accept(lfd, (struct sockaddr *)&client, &len);

	//获取客户端的IP和端口
	char ip[16];
	cout<<"client-->ip: "<<inet_ntop(AF_INET, &client.sin_addr.s_addr, ip, sizeof(ip))<<"  port: "<<ntohs(client.sin_port)<<endl;
	
	char buf[512];
	int size = 0;
	while(1)
	{
		//读取数据
		memset(buf, 0, sizeof(buf));
		size = read(cfd, buf, sizeof(buf));
		if(size<=0)
		{
			cout<<"read error or client close -- "<<size<<endl;
			break;
		}

		cout<<"recv data: "<<buf<<endl;

		//将接收到的数据转换成大写
		for(int i=0; i<size; i++)
		{
			buf[i] = toupper(buf[i]);
		}

		//将接收到的数据发送回客户端
		write(cfd, buf, size);
	}
	
	//关闭监听描述符和通信描述符
	close(lfd);
	close(cfd);

	return 0;
}
