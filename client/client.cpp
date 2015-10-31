#include "Aux.h"
#include "Login.h"

int main()
{
	Login login;

	login.runLoginThread();	

	while(1);
	return 0;
}
