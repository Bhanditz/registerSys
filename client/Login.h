
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

enum toClient { DefaultC='0', LoginSuccess, LoginFail, UserNotExist, PwdError};
enum toServer { DefaultS='0', KeepTry, TurnToRegister, Quit };

class Login
{

public:

	void loginProcess()
	{
		char flag;
		int sockfd;

		sockfd  = connectLoginService();
		showTitle();
		while(1) {
			recvMsg(buffer);
			flag = getBufferFlag();
			cout << buffer+1 << endl;			
			if(flag == LoginFail) exit(0);	
		}

	}


	Login() {}

	~Login() {}


private:

	/* the char buffer size limit is according to MYSQL*/
	//char user_name[256];
	//char password[256];

	// use for  communication with server	
	char buffer[257];
	char userInput[256];

	int connectLoginService()
	{
		return connectToServer(SERVER_IP, LOGIN_PORT);
	} 

	void sendMsg(const char* sendline)
	{
		int nwrite = 0;
		
		if((nwrite = write(connfd, sendline, strlen(sendline))) < 0) {
			throwError("[login]: write error");
		}	
	}

	void recvMsg(char* recvline)
	{
		int nread = 0;	
	
		bzero(recvline, sizeof(recvline));
		nread = read(connfd, recvline, sizeof(recvline));
		if(nread < 0) {
			throwError("[login]: read error");
		} else if(nread == 0) {
			cout << "\n[ERROR]: server has shut down" << endl;
			exit(0)
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

	void showTitle()
	{
		cout << "****************\n";
		cout << "*    LOG IN    *\n";
		cout << "****************\n";
	}	

};


#endif
