#include <iostream>

#include "RSmysql.h"
#include "unp.h"

int main()
{
	RSmysql mysql;

	mysql.init("ChatRoom");

	// run this kind of command, the return result ptr will be NULL
	mysql.createUserTable();
//	mysql.addNewUser("jun", "lolololo");
	if(mysql.isUserValid("jun", "lolololo")) cout << "password correct!" << endl;
	else cout << "password error!" << endl;

	return 0;
}
