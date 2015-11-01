
/*	Since use delete(this) in Service class function quit(),
	the Service object need to build with "new" operator,
	then after finishing service, each object will be killed by itself
	
	In the end of each serviceProcess() it will call quit() to release the 
	memory of this object
*/

#ifndef _SERVICE_H
#define _SERVICE_H

#include <iostream>
#include <thread>
#include <map>

#include "unp.h"
#include "Aux.h"

using namespace std;

class Service
{

public:

	/* Most of the running thread need to be passed the conn_table
	   to cancel the entry of the conn fd of this service when detect that client
		has shut down
	*/
	virtual void runServiceThread() 
	{
		serviceThread = thread(&Service::serviceProcess, this);	
		serviceThread.join();
		quit();
	}

	Service(int fd, map<int, int> *c_table, map<int, Service*> *s_table) 
	{
		connfd = fd;
		conn_table = c_table;
		service_table = s_table;
	}
   
	virtual ~Service() {}

protected:

	int		connfd;
	map<int, int> *conn_table;
	map<int, Service*> *service_table;

	thread	serviceThread;

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


};





#endif
