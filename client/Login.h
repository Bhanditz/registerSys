
/*	This class uses to deal with the process of user logging in,
	and also sets a timer, if user has long time without any operation,
	the connection to server will disconnect and need to log in agian.

	This class also has the information of client that abstract from server after
	user logs in successfully.

	Here login service use server port-<9511>
*/

#ifndef _LOGIN_H
#define _LOGIN_H

#include <iostream>

#include "Aux.h"
#include "unp.h"

class Login
{

public:

	void loginProcess()
	{
		cout << "----------------------------------------------------------------\n";
		cout << "                       L O G        I N                         \n";
		cout << "----------------------------------------------------------------\n";
		cout << "Please enter your user name: ";	
		scanfLine(user_name);
		
	}


	Login() {}

	~Login() {}


private:

	/* the char buffer size limit is according to MYSQL*/
	char user_name[256];
	char password[256];

	char email[256]; // this tag is to remind adding other info
	char gender[256];
	char lasttime_login[256];
	char thistime_login[256];

	bool verifyUserName(const char* user_name)
	{
		
	}

	bool verifyUserPwd(const char( user_name, const char* password)
	{

	}
};


#endif
