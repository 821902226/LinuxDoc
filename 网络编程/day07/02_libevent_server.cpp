#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <event.h>

using namespace std;

struct event *readev = NULL;

void readcb(evutil_socket_t fd, short events, void *arg)
{
	int n;
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	n = read(fd, buf, sizeof(buf));
	if(n<=0)
	{
		close(fd);
		event_del(readev);
		event_free(readev);
	}
	write(fd, buf, n);
}

//子线程回调函数
void concb(evutil_socket_t fd, short events, void *arg)
{
	struct event_base *base = (struct event_base *) arg;
	//接受新的客户端连接
	int cfd = accept(fd, NULL, NULL);
	if(cfd>0)
	{
		//创建通信文件描述符对应的事件并设置回调函数
		readev = event_new(base, cfd, EV_READ|EV_PERSIST, readcb, NULL);
		if(!readev)
		{
			exit(0);
		}

		//将通信文件描述福对应的事件上event_base地基
		event_add(readev, NULL);
	}
}

int main()
{
	//创建socket
	int lfd = socket(AF_INET, SOCK_STREAM, 0);

	//设置端口复用
	int opt_val = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(int));

	//绑定
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(9999);
	serv.sin_addr.s_addr = htonl(INADDR_ANY); //表示使用本机任意可用ip
	bind(lfd, (struct sockaddr *)&serv, sizeof(serv));

	//监听
	listen(lfd, 128);

	//创建地基
	struct event_base *base = event_base_new();
	if(!base)
	{
		cout<<"event_base_new error"<<endl;
		return -1;
	}

	//创建监听文件描述符对应的事件
	struct event *ev = event_new(base, lfd, EV_READ|EV_PERSIST, concb, base);
	if(!ev)
	{
		cout<<"event_new error"<<endl;
		return -1;
	}

	//将新的时间节点上base地基
	event_add(ev, NULL);

	//进入事件循环等待
	event_base_dispatch(base);

	//释放资源
	event_base_free(base);
	event_free(ev);
	close(lfd);

	return 0;
}
