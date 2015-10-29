#include "Aux.h"

int main()
{
	int listenfd = initiateMonitor(9511);

	acceptConnection(listenfd);

	while(1);

	return 0;
}
