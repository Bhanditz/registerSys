
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
#include <thread>
#include <semaphore.h>

#include "Aux.h"
#include "unp.h"

#define SERVER_IP	"127.0.0.1"
#define LOGIN_PORT	9511

enum toClient { DefaultC='0', LoginSuccess, LoginFail, UserNotExist, PwdError};
enum toServer { DefaultS='0', KeepTry, TurnToRegister, Quit };

class Login
{

public:

	void runLoginThread()
	{
		loginThread = thread(&Login::loginProcess, this);
		readThread = thread(&Login::recvMsg, this);	
	}

	Login() 
	{
		sockfd  = connectLoginService();
		sem_init(&sem_w, 0, 0);
		sem_init(&sem_r, 0, 1);
	}

	~Login() 
	{
		loginThread.join();
		readThread.join();
		sem_destroy(&sem_w);
		sem_destroy(&sem_r);
	}


private:

	/* the char buffer size limit is according to MYSQL*/
	//char user_name[256];
	//char password[256];

	int sockfd;

	// use for  communication with server	
	char sendline[MAXLINE];
	char recvline[MAXLINE];

	thread loginThread;
	
	/* use to receive the msg sent from server.
		use one another thread to listen to the server because it can
		monitor the server condition at real time, such as server 
		shut down
	*/
	thread readThread;
	
	/* use for the exclusive of login thread and read thread
		separate set in loginProcess() and recvMsg()
	*/
	sem_t sem_w;
	sem_t sem_r;


	void loginProcess()
	{
	cout << "== enter login thread ==" << endl;
		char input[MAXLINE];
	
		sockfd  = connectLoginService();
		showTitle();

		while(1) {
			cout << "== block in write sem ==" << endl;
			sem_wait(&sem_w);
			cout << "== get the write sem ==" << endl;
			checkFlag();
			cout << "recvline: " << recvline+1 << endl;
			showMsg();
			sem_post(&sem_r);
			cout << "== posts for read sem ==" << endl; 
			getUserInput(input, sizeof(input));
			setBufferFlag(DefaultS);
			setBufferData(input);
			sendMsg();			
		}
	}

	int connectLoginService()
	{
		return connectToServer(SERVER_IP, LOGIN_PORT);
	} 

	void getUserInput(char *input, size_t len)
	{
		while(1) {
			fgets(input, len, stdin);
			input[strlen(input)-1] = '\0';
			if(strlen(input) > 255) {
				cout << "[LOGIN]: Input too long" << endl;
				cout << " Please enter again: "; 
			} else {
				return;
			}
		} 
	}

	void checkFlag() 
	{
		char flag = getBufferFlag();

		/* overlook the Default flag here */
		if(flag == UserNotExist) {
			cout << "[LOGIN]: User name is not existed" << endl;
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

	void showMsg() 
	{
		cout << endl;
		cout << recvline+1 << endl;
	}

	void sendMsg()
	{
		int nwrite = 0;
		cout << "length of sendline: " << strlen(sendline) << endl;	
		if((nwrite = write(sockfd, sendline, strlen(sendline))) < 0) {
			throwError("[login]: write error");
		}	

		cout << "Send out " << nwrite << "-bytes data" << endl;
	}

	void recvMsg()
	{
	cout << "== enter recv msg thread ==" << endl;
		int nread = 0;	

		while(1) {	
			cout << "== block in read sem ==" << endl;
			sem_wait(&sem_r);
			cout << "== get the read sem ==" << endl;
			bzero(recvline, sizeof(recvline));
			cout << "waiting for incoming msg..." << endl;
			nread = read(sockfd, recvline, sizeof(recvline));
			if(nread < 0) {
				throwError("[login]: read error");
			} else if(nread == 0) {
				cout << "\n[ERROR]: server has shut down" << endl;
				exit(0);
			}
			cout << "Receive " << nread << "-bytes data" << endl;
			sem_post(&sem_w);
			cout << "== post for write sem ==" << endl;
		}
	}
	
	void setBufferData(const char* data) 
	{
		bzero(sendline+1, sizeof(sendline)-1);
		strcpy(sendline+1, data);
		cout << "****copy " << strlen(data)  << "-bytes data in to sendline" << endl;
	}
	
	void setBufferFlag(const char c)
	{
		sendline[0] = c;	
	}	

	char getBufferFlag()
	{
		return recvline[0];
	}


};


#endif
