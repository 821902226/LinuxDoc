#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>

using namespace std;

int main()
{
	//创建socket
	int cfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(cfd<0)
	{
		perror("socket error");
		return -1;
	}

	//绑定
	struct sockaddr_in serv, client;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(8888);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(cfd, (struct sockaddr *)&serv, sizeof(serv));

	int size;
	char buf[1024];
	socklen_t len;
	while(1)
	{
		//读取数据
		memset(buf, 0, sizeof(buf));
		len =  sizeof(client);
		size = recvfrom(cfd, buf, sizeof(buf), 0, (struct sockaddr*)&client, &len);

		//将大写转换为小写
		for(int i=0; i<size; i++)
		{
			buf[i] = toupper(buf[i]);
		}

		//发送数据
		sendto(cfd, buf, size, 0, (struct sockaddr*)&client, len);
	}

	//关闭套接字
	close(cfd);

}
