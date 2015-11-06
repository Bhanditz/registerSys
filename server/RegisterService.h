/*	use to set up the username, password and record the user 
	data into mysql, so that it can pass the login service.
	Here connect to two tables, first is to the Users and UserData
*/

#ifndef _REGISTERSERVICE_H
#define _REGISTERSERVICE_H

#include "SService.h"
#include "RSLSmysql.h"


class RegisterService : public SService
{

public:

	void runServiceThread()
	{
		serviceThread = thread(&RegisterService::serviceProcess, this);
		serviceThread.join();
		quit();
	}

	RegisterService(int fd, map<int, int> *c_table, map<int, SService*> *s_table) 
		: SService(fd, c_table, s_table) 
	{
		mysql.init("ChatRoom");
		mysql.createUserTable();
	}


private:

	RSLSmysql mysql;


	/*	the setting should divide into two sections:
		first is to set up the username and password, and store in to "Users";
		second is to set up the user infomation, and store in to "UserData";
	*/
	void serviceProcess()
	{
		if(registerUserAccount() < 0) {
			notifyFail();
			return;
		}	

		informClient("Follow up please enter some personal infomation...");

		furtherInfo();
	
		notifySuccess();	
	}

	/*	first confirm the username is feasible, then to set up the password, and
		password does not need to check, the complexity of password should be check
		in client side
		-1 means quit, 0 means normal 
	*/
	int registerUserAccount()
	{	
		char password[256];
		bool res = setUserName(UserName);	
		while(res) {
			res = setUserName(UserExist);
			if(checkRespFlag() < 0) return -1;
		}
		strcpy(username, buffer+1);

		setPassword(Password);	
		strcpy(password, buffer+1);
		if(strlen(password) < 6) return -1;	

		mysql.addNewUser(username, password);

		return 0;
	}

	void furtherInfo()
	{
		char attribute[] = {Gender, Email, Phone, Street, City, State, Zipcode};
		string name[] = {"gender", "e-mail", "phone number", "street", "city", "state", "zip code"};
	
		char content[MAXLINE];
		char response[8][MAXLINE];
		strcpy(response[0], username);
		for(int i=0; i<7; i++) {
			bzero(content, sizeof(content));
			snprintf(content, MAXLINE, "Set up your %s: ", name[i].c_str());
			setBufferFlag(attribute[i]);	
			setBufferData(content);	
			sendMsg(buffer);
			recvMsg(buffer, sizeof(buffer));
			strcpy(response[i+1], buffer+1);
		}	
		mysql.addUserInfo(response);			
	}

	void informClient(const char* content) 
	{
		setBufferFlag(DontReply);
		setBufferData(content);
		sendMsg(buffer);
	}

	void notifyFail() 
	{
		setBufferFlag(LoginFail);
		setBufferData("Register failed...");
		sendMsg(buffer);
		cout << "Client<" << username << "> register failed..." << endl;
	}

	void notifySuccess()
	{
		setBufferFlag(LoginSuccess);
		setBufferData("Register Successful!");
		sendMsg(buffer);
		cout << "Client<" << username << "> register successfully" << endl;
	}

	bool setUserName(const char flag)
	{
		setBufferFlag(flag);
		setBufferData("Set up your username: ");	
		sendMsg(buffer);
		recvMsg(buffer, sizeof(buffer));
		return mysql.isUserExist(buffer+1);
	}

	/* password complexity judge in the client side */
	void setPassword(const char flag)
	{
		if(!strlen(username)) {
			throwError("[login]: set up username first");
		}
		setBufferFlag(flag);
		setBufferData("Set up your password: ");
		sendMsg(buffer);
		recvMsg(buffer, sizeof(buffer));
	} 
	
	int checkRespFlag()
	{
		char flag = getBufferFlag();
		if(flag == Quit) {
			return -1;
		}
		return 0;
	} 

	/*	the judge of user input take in client side */
	void promptRequest(const char flag, const char *req)	
	{
		setBufferFlag(flag);
		setBufferData(req);
		sendMsg(buffer);
		recvMsg(buffer, sizeof(buffer));
	}
	

};


#endif
