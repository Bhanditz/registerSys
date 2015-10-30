
/*  use a struct to store the flags that control the process, and the info append on it.	
	|--some flags --| ------------ data --------------|     <-- the packet end out;
	And the prompt in user interface is controlled and set by server side as well.
*/

#ifndef _LOGINSERVICE_H
#define _LOGINSERVICE_H

#include "Service.h"
#include "RSLSmysql.h"

/* Communication:	for client: 0> default 1> keep trying 2> turn to register(quit) 3> quit
					for server: 0> default 1> login success 2> login fail 3> username not exist $> password error
*/
enum toClient { DefaultC='0', LoginSuccess, LoginFail, UserNotExist, PwdError};
enum toServer { DefaultS='0', KeepTry, TurnToRegister, Quit};

class LoginService : public Service
{

public:

	void runServiceThread()
	{
		serviceThread = thread(&LoginService::serviceProcess, this, conn_table);
	}

	LoginService(int fd, map<int, int> *c_table, map<int, Service*> *s_table) 
		: Service(fd, c_table, s_table) {}


private:

	RSLSmysql mysql;	

	char buffer[257];   // use to extract the flag and data
	char username[256];


	void serviceProcess(map<int, int> *conn_table)
	{
		int count = 1;
		bool res = checkUserName(DefaultC);	
		while(!res) {
			if(count == 3) overMaxTrying();
			res = checkUserName(ChkFail);
			count++;
		}
		strcpy(username, buffer+1);
		
		count = 1;
		res = checkPassword(ChkSuccess);	
		while(!res) {
			if(count == 3) overMaxTrying();
			res = checkPassword(ChkFail);
			count++;
		}

		//inform the client log in success
		notifySuccess();	

		quit();					
	}

	/* in the fucture development, only when the login success, 
		the user will be promise to connect to the server's other
		services
	*/
	void notifySuccess()
	{
		setBufferFlag(LoginSuccess);	
		setBufferData("Login Successful!");
		sendMsg(buffer);
	}
	
	// pass the check function into it
	void checkElement(const char flag, bool (*checkFunc)(const char flag))
	{
		int count = 1;
		bool res = checkFunc(flag);	
		while(!res) {
			if(count == 3) overMaxTrying();
			res = checkFunc(ChkFail);
			count++;
		}

	}

	void overMaxTrying()
	{
		setBufferFlag(LoginFail);
		setBufferData("Over maximum trying times!");
		sendMsg(buffer);
		quit();
	}

	bool checkUserName(const char flag)
	{
		setBufferFlag(flag);
		setBufferData("Please enter your username: ");	
		sendMsg(buffer);
		recvMsg(buffer);
		checkRespFlag();
		return mysql.isUserExist(buffer+1);
	}

	bool checkPassword(const char flag)
	{
		if(!strlen(username)) {
			throwError("[login]: check username first");
		}
		setBufferFlag(flag);
		setBufferData("Please enter your password: ");
		sendMsg(buffer);
		recvMsg(buffer);
		checkRespFlag();
		return mysql.isPwdValid(username, buffer+1);
	} 

	// check the flag client set in the respond
	void checkRespFlag()
	{
		char flag = getBufferFlag();	
		if(flag == TurnToRegister || flag == Quit) {
			quit();
		}	
	}

	void setBufferData(const char* data) 
	{
		bzero(buffer+1, sizeof(buffer)-1);
		strcpy(buffer+1, data);
	}
	
	void setBufferFlag(const char c)
	{
		buffer[0] = c;	
	}	

	char getBufferFlag()
	{
		return buffer[0];
	}
	
};


#endif
