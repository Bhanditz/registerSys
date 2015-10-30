
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

#define SERVER_IP	"127.0.0.1"
#define LOGIN_PORT	9511

class Login
{

public:

	void loginProcess()
	{
		int sockfd = connectLoginService();

		cout << "----------------------------------------------------------------\n";
		cout << "                       L O G        I N                         \n";
		cout << "----------------------------------------------------------------\n";
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

	int connectLoginService()
	{
		return connectToServer(SERVER_IP, LOGIN_PORT);
	} 

};


#endif
