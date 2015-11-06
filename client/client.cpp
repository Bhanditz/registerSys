#include "Aux.h"
#include "unp.h"
#include "Login.h"
#include "Register.h"

int main()
{
	CService *service = new Register(REGIS_PORT);

	service->runServiceThread();	

	while(1);
	return 0;
}
