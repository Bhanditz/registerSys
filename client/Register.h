#ifndef _REGISTER_H
#define _REGISTER_H

#include "CService.h"

class Register : public CService
{

public:

	void runServiceThread()
	{
		serviceThread = thread(&Register::serviceProcess, this);
		readThread = thread(&Register::recvMsg, this);

		serviceThread.join();
		readThread.join();
		quit();
	}
	
	Register(int port) : CService(port) {}


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
			if(getBufferFlag() == DontReply) {
				cout << endl;
				continue;
			}
			if(getBufferFlag() != Password) {
				getUserInput(input, sizeof(input));
			} else {
				setPassword(input, sizeof(input));
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
		if(flag == UserExist) {
			cout << "[REGISTER]: Username has been existed" << endl;
			showChoice();
		} else if(flag == RegisterFail) {
			cout << "[REGISTER]: Register fail, infomation filling is not completed..." << endl;
			cout << "[REGISTER]: Waiting for System exit..." << endl;
			cout << endl;
			exit(0);
		} else if(flag == RegisterSuccess) {
			// turn to the main service interface		
		}			
	}

	/* the password need to type in two times to confirm*/
	void setPassword(char *input, size_t len)
	{
		char first[MAXLINE], second[MAXLINE];	
		
		while(true) {
			readPassword(first, sizeof(first));
			cout << "Please enter the password again: ";
			readPassword(second, sizeof(second));
			if(strlen(first) < 6) {
				cout << "[ERROR]: Password too short, it must contain at least 6 characters..." << endl;
				cout << "Set up the password: ";
				continue;
			} 
			// if two inputs are the same, then break
			if(strcmp(first, second)) {
				cout << "[ERROR]: Passwords typing are not the same..." << endl;
				cout << "Set up the password: ";
				continue;
			} 
			break;
		}	

		strcpy(input, first);
	}
	
	void showChoice()
	{
		char input[MAXLINE]; 
		
		while(1) {
			cout << "---------------------" << endl;	
			cout << "1> Keep Trying"		<< endl; 
			cout << "2> Quit"				<< endl;
			cout << "---------------------" << endl;
			cout << "Enter your choice: ";
			fgets(input, MAXLINE, stdin);
			input[strlen(input)-1] = '\0';
			if(!strncmp(input, "1", strlen(input))) {
				setBufferFlag(KeepTry);
				break;	
			} else if(!strcmp(input, "2")) {
				cout << "System is exiting..." << endl;
				setBufferFlag(Quit);
				exit(0);
			} else {
				cout << "Invalid input: please enter 1 or 2" << endl;
			}	
		}
	}

	void showTitle()
	{
		cout << "****************" << endl;
		cout << "*   REGISTER   *" << endl;
		cout << "****************" << endl;
	}

};


#endif
