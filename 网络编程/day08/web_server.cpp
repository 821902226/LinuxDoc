#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <sstream>
#include <dirent.h>
#include "pub.h"
#include "wrap.h"	//自定义文件，对socket相关函数进行封装，异常处理

using namespace std;

int send_header(int cfd, string code, string msg, string fileType, int len)
{
	string head = string("HTTP/1.1 ")+code+string(" ")+msg+string("\r\n");
	head += string("Content-Type: ")+fileType+string("\r\n");
	if(len>0)
	{
		head += string("Content-Length:")+to_string(len)+string("\r\n");
	}

	head += string("\r\n");
	char *str = head.data();
	Write(cfd, str, strlen(str));
}

int send_file(int cfd, char *fileName)
{
	//打开文件
	int fd = open(fileName, O_RDONLY);
	if(fd<0)
	{
		perror("open error");
		return -1;
	}

	//循环读取文件， 然后发送
	char buf[1024];
	while(1)
	{
		memset(buf, 0, sizeof(buf));
		int n = Read(fd, buf, sizeof(buf));
		if(n<=0)
		{
			break;
		}
		else
		{
			Write(cfd, buf, n);
		}
	}
}

int http_request(int cfd, int epfd)
{
	int n;
	char buf[1024];
	//读取请求行数据，分析要请求的资源文件名
	memset(buf, 0, sizeof(buf));
	n = Readline(cfd, buf, sizeof(buf));
	if(n<=0)
	{
		//关闭文件描述符
		close(cfd);
		//将文件描述符冲epoll树上下树
		epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
		return -1;
	}
	//GET /test.cpp HTTP/1.1
	string firstLine(buf);
	string reqType, fileName, protocal;
	istringstream head(firstLine);
	head>>reqType;
	head>>fileName;
	head>>protocal;
	if(fileName.length()<=1)
	{
		//默认访问工作目录
		fileName = "./";
	}
	else
	{
		fileName = fileName.substr(1);
	}

	//循环读取剩下的数据
	while((n=Readline(cfd, buf, sizeof(buf)))>0);

	//判断文件是否存在
	struct stat st;
	//若文件不存在
	if(stat(fileName.data(), &st)<0)
	{
		//组织应答信息：http响应消息+错误页内容
		send_header(cfd, "404", "NOT FOUND", get_mime_type(".html"), 0);
		send_file(cfd, "error.html");
	}
	//若文件存在
	else
	{
		//判断文件类型
		//普通文件
		if(S_ISREG(st.st_mode))
		{
			//发送头部信息
			send_header(cfd, "200", "OK", get_mime_type(fileName.data()), st.st_size);
			//发送文件内容
			send_file(cfd, fileName.data());
		}
		//目录文件
		else if(S_ISDIR(st.st_mode))
		{
			send_header(cfd, "200", "OK", get_mime_type(".html"), 0);
			
			send_file(cfd, "./dir_header.html");
			//文件列表信息
			struct dirent **namelist;
			char buffer[1024];
			n = scandir(fileName.data(), &namelist, NULL, alphasort);
			if(n<0)
			{
				close(cfd);
				epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
				perror("scandir error");
			}
			else
			{
				while(n--)
				{
					memset(buffer, 0, sizeof(buffer));
					if(namelist[n]->d_type==DT_DIR)
					{
						sprintf(buffer, "<li><a href=%s/> %s </a></li>", namelist[n]->d_name, namelist[n]->d_name);
					}
					else
					{
						sprintf(buffer, "<li><a href=%s> %s </a></li>", namelist[n]->d_name, namelist[n]->d_name);
					}
					free(namelist[n]);
					Write(cfd, buffer, strlen(buffer));
				}
				free(namelist);
			}

			send_file(cfd, "./dir_tail.html");
		}
	}
}

int main()
{
	//改变当前进程的工作目录
	char path[255] = "/home/feng/linux_code/web_server";
	chdir(path);

	//创建socket 
	int lfd = Socket(AF_INET, SOCK_STREAM, 0);

	//设置端口复用
	int opt_val = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(int));

	//绑定
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(8899);
	serv.sin_addr.s_addr = htonl(INADDR_ANY); //表示使用本机任意可用ip
	Bind(lfd, (struct sockaddr *)&serv, sizeof(serv));

	//监听
	Listen(lfd, 128);

	//创建一颗eopll树
	int epfd = epoll_create(1);
	if(epfd==-1)
	{
		perror("Create epoll error!");
		close(lfd);
		exit(-1);
	}

	//将监听描述符上树
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = lfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
	
	int flag;
	struct epoll_event evs[1024];
	while(1)
	{
		memset(evs, 0, sizeof(evs));
		flag = epoll_wait(epfd, evs, 1024, -1); // -1 表示永久阻塞,直到有事件发生
		if(flag==-1)
		{
			if(errno==EINTR)	//被信号中断
			{
				continue;
			}
			perror("epoll error");
			break;
		}

		//遍历所有发生的事件
		for(int i=0; i<flag; i++)
		{
			int sockfd = evs[i].data.fd;
			//有客户端请求链接
			if(sockfd==lfd)
			{
				int cfd = Accept(lfd, NULL, NULL);
				//设置cfd为非阻塞
				int flag = fcntl(cfd, F_GETFL);
				flag |= O_NONBLOCK;
				fcntl(cfd, F_SETFL, flag);
				//上epoll树
				ev.events = EPOLLIN;
				ev.data.fd = cfd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
			}
			//有客户端发送数据
			else
			{
				http_request(sockfd, epfd);
			}
		}
	}
	
	//关闭树根描述符
	close(epfd);
	//关闭监听描述符
	close(lfd);

	return 0;
}
