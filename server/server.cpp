#include "Aux.h"
#include "LoginService.h"
#include "RegisterService.h"

#include <map>

using namespace std;

int main()
{
	int listenfd = initiateMonitor(9512);

	int connfd = acceptConnection(listenfd);

	map<int, int> c_table;
	map<int, SService*> s_table;
	SService *service = new RegisterService(connfd, &c_table, &s_table);
	service->runServiceThread();

	while(1);

	return 0;
}
