#include <iostream>
#include <string.h>
#include <event2/event.h>

using namespace std;

int main()
{
	int i=0;
	//获取当前系统支持的方法
	const char **p = event_get_supported_methods();
	while(p[i]!=NULL)
	{
		cout<<p[i++]<<"\t"<<endl;
	}
	cout<<endl;

	//获取地基节点
	struct event_base * base = event_base_new();
	//获取当前系统使用的方法
	const char *p1 = event_base_get_method(base);
	cout<<p1<<endl;
	//释放地基节点
	event_base_free(base);

}
