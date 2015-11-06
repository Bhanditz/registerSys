#ifndef _CSERVICE_H
#define _CSERVICE_H

#include <iostream>
#include <thread>
#include <semaphore.h>

#include "Aux.h"
#include "unp.h"
#include "ReadPwd.h"

#define SERVER_IP	"127.0.0.1"
#define MENU_PORT   9510
#define LOGIN_PORT	9511
#define REGIS_PORT  9512

enum toClient { DefaultC='0', LoginSuccess, LoginFail, RegisterSuccess, RegisterFail, UserNotExist, PwdError, UserExist, DontReply };
enum toServer { DefaultS='0', KeepTry, TurnToLogin, TurnToRegister, Quit };
enum attributeType { UserName='a', Password, EMail, Gender, Phone, Street, City, State, ZipCode };


class CService 
{

public:

	virtual void runServiceThread()
	{
		serviceThread = thread(&CService::serviceProcess, this);
		readThread = thread(&CService::recvMsg, this);	

		serviceThread.join();
		readThread.join();
		/* the service object release memory by itself
			quit() use delete(this) here
		*/
		quit();
	}

	CService(int port) 
	{
		sockfd  = connectService(port);
		sem_init(&sem_w, 0, 0);
		sem_init(&sem_r, 0, 1);
	}

	virtual ~CService() 
	{
		sem_destroy(&sem_w);
		sem_destroy(&sem_r);
	}


protected:

	int sockfd;

	// use for  communication with server	
	char sendline[MAXLINE];
	char recvline[MAXLINE];

	thread serviceThread;
	
	/* use to receive the msg sent from server.
		use one another thread to listen to the server because it can
		monitor the server condition at real time, such as server 
		shut down
	*/
	thread readThread;
	
	/* use for the exclusive of service thread and read thread
		separate set in serviceProcess() and recvMsg()
	*/
	sem_t sem_w;
	sem_t sem_r;


	virtual void serviceProcess() {}
	
	virtual void showTitle() {}

	virtual void checkFlag() {}

	virtual void showChoice() {}

	int connectService(const int port)
	{
		return connectToServer(SERVER_IP, port);
	} 

	void quit() 
	{
		close(sockfd);
		delete(this);
	}

	void getUserInput(char *input, size_t len)
	{
		while(1) {
			bzero(input, sizeof(input));
			fgets(input, len, stdin);
			input[strlen(input)-1] = '\0';
			if(strlen(input) > 255) {
				cout << "[CService]: Input too long" << endl;
				cout << " Please enter again: "; 
			} else {
				return;
			}
		} 
	}

	void sendMsg()
	{
		int nwrite = 0;
		if((nwrite = write(sockfd, sendline, strlen(sendline))) < 0) {
			throwError("[CService]: write error");
		}	
#ifdef _DEBUG
		cout << "Send out " << nwrite << "-bytes data" << endl;
#endif
	}

	/* recvMsg() run in a separated thread to keep in touch with
		server connection
	 */
	void recvMsg()
	{
		int nread = 0;	

		while(1) {	
			sem_wait(&sem_r);
			bzero(recvline, sizeof(recvline));
			nread = read(sockfd, recvline, sizeof(recvline));
			if(nread < 0) {
				throwError("[CService]: read error");
			} else if(nread == 0) {
				cout << "\n[ERROR]: server has shut down" << endl;
				exit(0);
			}
#ifdef _DEBUG
			cout << "Receive " << nread << "-bytes data" << endl;
#endif
			sem_post(&sem_w);
		}
	}

	void showMsg() 
	{
		cout << recvline+1;
	}

	void setBufferData(const char* data) 
	{
		bzero(sendline+1, sizeof(sendline)-1);
		strcpy(sendline+1, data);
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
