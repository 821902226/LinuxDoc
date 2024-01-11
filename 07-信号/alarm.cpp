#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

static int i=0;

void hander(int signo)
{
	cout<<"计数到： "<<i<<endl;
	alarm(0);
	return;
}

int main()
{
	signal(SIGALRM, hander);
	alarm(1);
	while(1)
	{
		i++;
	}	

	return 0;
}
