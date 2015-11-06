/* mainly to check the validity of username and password
	the user enter and to let user into the chatroom service.
	After login successful, it should produce a user object to
	go on the other service 
*/

#ifndef _LOGINSERVICE_H
#define _LOGINSERVICE_H

#include "SService.h"
#include "RSLSmysql.h"

#define TRY_NUM		3


class LoginService : public SService
{

public:

	void runServiceThread()
	{
		serviceThread = thread(&LoginService::serviceProcess, this);
		serviceThread.join();
		quit();
	}

	LoginService(int fd, map<int, int> *c_table, map<int, SService*> *s_table) 
		: SService(fd, c_table, s_table) 
	{
		mysql.init("ChatRoom");	
		mysql.createUserTable();
	}


private:

	RSLSmysql mysql;	


	void serviceProcess()
	{
		//check the username, most for 3 times
		int count = 1;
		bool res = checkUserName(DefaultC);	
		while(!res) {
			if(count == TRY_NUM) {
				overMaxTrying();
				return;
			}
			res = checkUserName(UserNotExist);
			if(checkRespFlag() < 0) return;
			count++;
		}
		strcpy(username, buffer+1);
		
		//check the password, most for 3 times
		count = 1;
		res = checkPassword(Password);	
		while(!res) {
			if(count == TRY_NUM) {
				overMaxTrying();
				return;
			}
			res = checkPassword(PwdError);
			if(checkRespFlag() < 0) return;
			count++;
		}

		//inform the client log in success
		notifySuccess();	
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
		cout << "Client<" << username << "> login successfully" << endl;
	}
	
	void overMaxTrying()
	{
		setBufferFlag(LoginFail);
		setBufferData("Over maximum trying times!");
		sendMsg(buffer);
	}

	bool checkUserName(const char flag)
	{
		setBufferFlag(flag);
		setBufferData("Please enter your username: ");	
		sendMsg(buffer);
		recvMsg(buffer, sizeof(buffer));
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
		recvMsg(buffer, sizeof(buffer));
		return mysql.isPwdValid(username, buffer+1);
	} 

	// check the flag client set in the respond
	int checkRespFlag()
	{
		char flag = getBufferFlag();	
		if(flag == TurnToRegister || flag == Quit) {
			return -1;
		}	
		return 0;
	}

};


#endif
