/*	This class uses to deal with the process of user logging in,
	and also sets a timer, if user has long time without any operation,
	the connection to server will disconnect and need to log in agian.

	This class also has the information of client that abstract from server after
	user logs in successfully.

	Here login service use server port-<9511>
*/

#ifndef _LOGIN_H
#define _LOGIN_H

#include "CService.h"


class Login : public CService
{

public:

	void runServiceThread()
	{
		serviceThread = thread(&Login::serviceProcess, this);
		readThread = thread(&Login::recvMsg, this);	

		serviceThread.join();
		readThread.join();
		quit();
	}

	Login(int port) : CService(port) {}  


private:

	void serviceProcess()
	{
		char input[MAXLINE];

		showTitle();

		while(1) {
			sem_wait(&sem_w);
			checkFlag();
			showMsg();
			sem_post(&sem_r);
			if(!isPwdFlag()) {
				getUserInput(input, sizeof(input));
			} else	{
				readPassword(input, sizeof(input));
			}
			setBufferFlag(DefaultS);
			setBufferData(input);
			sendMsg();			
		}
	}

	void checkFlag() 
	{
		char flag = getBufferFlag();

		/* overlook the Default flag here */
		if(flag == UserNotExist) {
			cout << "[LOGIN]: Username is not existed" << endl;
			showChoice();
		} else if(flag == PwdError) {
			cout << "[LOGIN]: Password is error" << endl;
			showChoice();
		} else if(flag == LoginFail) {
			cout << "[LOGIN]: Login fail..." << endl;
			cout << "[LOGIN]: Waiting for System exit..." << endl;
			cout << endl;
			exit(0);
		} else if(flag == LoginSuccess) {
			// turn to the main service interface		
		}  	
	}

	void showChoice()
	{
		char input[MAXLINE]; 
		
		while(1) {
			cout << "---------------------" << endl;	
			cout << "1> Keep Trying"		<< endl; 
			cout << "2> Go To Get Register" << endl;
			cout << "3> Quit"				<< endl;
			cout << "---------------------" << endl;
			cout << "Enter your choice: ";
			fgets(input, MAXLINE, stdin);
			input[strlen(input)-1] = '\0';
			if(!strncmp(input, "1", strlen(input))) {
				setBufferFlag(KeepTry);
				break;	
			} else if(!strcmp(input, "2")) {
				cout << "Go to register module" << endl;
				setBufferFlag(TurnToRegister);
				// here tmporary use exit 
				exit(0);
			} else if(!strcmp(input, "3")) {
				cout << "System is exiting..." << endl;
				setBufferFlag(Quit);
				exit(0);
			} else {
				cout << "Invalid input: please enter 1, 2 or 3" << endl;
			}	
		}
	}

	void showTitle()
	{
		cout << "****************" << endl;
		cout << "*    LOG IN    *" << endl;
		cout << "****************" << endl;
	}	

	bool isPwdFlag()
	{
		char c = getBufferFlag();
		if(c == Password || c == PwdError) return true;	
		return false;
	}


};


#endif
