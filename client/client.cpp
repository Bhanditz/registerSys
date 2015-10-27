#include "Aux.h"

#define SERVER_IP "127.0.0.1"

int main()
{
	connectToServer(SERVER_IP, 9510);	
	while(1);

	return 0;
}
