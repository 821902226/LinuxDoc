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
	int cfd = socket(AF_INET, SOCK_STREAM, 0);
	if(cfd<0)
	{
		perror("socket error");
		return -1;
	}

	//连接服务器
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(9999);
	inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.s_addr);
	int ret = connect(cfd, (struct sockaddr *)&serv, sizeof(serv));
	if(ret<0)
	{
		perror("connect error");
		return -1;
	}

	char buf[512];
	int size = 0;
	while(1)
	{
		//读取数据
		memset(buf, 0, sizeof(buf));
		size = read(STDIN_FILENO, buf, sizeof(buf));

		//将数据发送给服务器
		write(cfd, buf, size);

		//读取服务器发来的数据
		memset(buf, 0, sizeof(buf));
		size = read(cfd, buf, sizeof(buf));
		if(size<=0)
		{
			cout<<"read error or server close -- "<<size<<endl;
			break;
		}
		cout<<"size = "<<size<<"  buf = "<<buf<<endl;
	}
	
	//关闭通信描述符
	close(cfd);

	return 0;
}
