#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

using namespace std;

int i=0;

void hander(int signo)
{
	i++;
	cout<<"计数到： "<<i<<endl;
}

int main()
{
	signal(SIGALRM, hander);

	struct itimerval tm;
	tm.it_interval.tv_sec = 1;
	tm.it_interval.tv_usec = 0;

	tm.it_value.tv_sec = 3;
	tm.it_value.tv_usec = 0;

	int flag = setitimer(ITIMER_REAL, &tm, NULL);
	
	while(1);

	return 0;
}
