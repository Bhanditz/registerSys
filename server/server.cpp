#include "Aux.h"
#include "LoginService.h"

#include <map>

using namespace std;

int main()
{
	int listenfd = initiateMonitor(9511);

	int connfd = acceptConnection(listenfd);

	map<int, int> c_table;
	map<int, Service*> s_table;
	Service service(connfd, &c_table, &s_table);
	service.runServiceThread();

	while(1);

	return 0;
}
