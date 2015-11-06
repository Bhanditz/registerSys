/*	Since use delete(this) in Service class function quit(),
	the Service object need to build with "new" operator,
	then after finishing service, each object will be killed by itself
	
	In the end of each serviceProcess() it will call quit() to release the 
	memory of this object

	use a struct to store the flags that control the process, and the info append on it.	
	|--some flags --| ------------ data --------------|     <-- the packet end out;
	And the prompt in user interface is controlled and set by server side as well.
*/


#ifndef _SSERVICE_H
#define _SSERVICE_H

#include <iostream>
#include <thread>
#include <map>

#include "unp.h"
#include "Aux.h"

using namespace std;

enum toClient { DefaultC='0', LoginSuccess, LoginFail, RegisterSuccess, RegisterFail, UserNotExist, PwdError, UserExist, DontReply };
enum toServer { DefaultS='0', KeepTry, TurnToLogin, TurnToRegister, Quit };
enum attributeType { UserName='a', Password, Email, Gender, Phone, Street, City, State, Zipcode };


class SService
{

public:

	/* Most of the running thread need to be passed the conn_table
	   to cancel the entry of the conn fd of this service when detect that client
		has shut down
	*/
	virtual void runServiceThread() 
	{
		serviceThread = thread(&SService::serviceProcess, this);	
		serviceThread.join();
		quit();
	}

	SService(int fd, map<int, int> *c_table, map<int, SService*> *s_table) 
	{
		connfd = fd;
		conn_table = c_table;
		service_table = s_table;
	}
   
	virtual ~SService() {}

protected:

	int		connfd;
	map<int, int> *conn_table;
	map<int, SService*> *service_table;

	// use to launch the service
	thread	serviceThread;

	// use to extract the flag and data
	char buffer[MAXLINE];

	// the 256 lenght limit now is set in client
	char username[256];


	virtual void serviceProcess() 
	{
		cout << "[Service]: empty service process method" << endl; 
	}
	
	/* the service active quit
		quit() cannot be used inside the thread dut to delete(this) 
	*/
	void quit()
	{
		close(connfd);
		deleteConnfdFromTable();
		deleteServiceFromTable();
		// delete this service object when finish this service thread
		delete(this);
	}

	void sendMsg(const char* sendline)
	{
		int nwrite = 0;
		
		if((nwrite = write(connfd, sendline, strlen(sendline))) < 0) {
			throwError("[Service]: write error");
		}	
		cout << "Send out " << nwrite << "-bytes data..." << endl; 
	}

	void recvMsg(char* recvline, size_t len)
	{
		int nread = 0;	
	
		bzero(recvline, len);
		cout << "waiting for incoming msg" << endl;
		nread = read(connfd, recvline, len);
		if(nread < 0) {
			throwError("[Service]: read error");
		} else if(nread == 0) {
			cout << "\n[WARNING]: client has shut down" << endl;
			processClientDown();
		}
		cout << "Receive " << nread  << "-bytes data" << endl;
	}

	/* use to delete the conn fd in table when the detect the client shut down */
	void processClientDown()
	{
		if(conn_table->find(connfd) != conn_table->end()) {
			cout << "\n[INFO]: Client<" << conn_table->at(connfd) << ">  has shut down" << endl;
		}
		// delete the corresponding fd in table
		deleteConnfdFromTable();
	}

	void deleteServiceFromTable()
	{
		if(service_table->find(connfd) != service_table->end()) {
			service_table->erase(service_table->find(connfd));
		}
	}

	void deleteConnfdFromTable()
	{
		if(conn_table->find(connfd) != conn_table->end()) {
			conn_table->erase(conn_table->find(connfd));
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
		cout << "recv flag: " << buffer[0] << endl;
		return buffer[0];
	}
	
};





#endif
