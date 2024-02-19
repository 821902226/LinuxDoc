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

	//指定服务器
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(8888);
	inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.s_addr);

	int size;
	char buf[1024];
	socklen_t len;
	while(1)
	{
		//读取标准输入
		memset(buf, 0, sizeof(buf));
		size = read(STDIN_FILENO, buf, sizeof(buf));
		//发送数据
		sendto(cfd, buf, size, 0, (struct sockaddr*)&serv, sizeof(serv));

		//读取数据
		memset(buf, 0, sizeof(buf));
		size = recvfrom(cfd, buf, sizeof(buf), 0, NULL, NULL);
		cout<<"Read Data: "<<buf<<endl;

	}

	//关闭套接字
	close(cfd);

}
