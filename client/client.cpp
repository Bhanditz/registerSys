#include "Aux.h"
#include "Login.h"

#define SERVER_IP "127.0.0.1"

int main()
{
	connectToServer(SERVER_IP, 9511);	

	Login login;
	login.loginProcess();	

	while(1);

	return 0;
}
