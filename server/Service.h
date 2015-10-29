#ifndef _SERVICE_H
#define _SERVICE_H

#include <iostream>
#include <thread>
#ifndef _SERVICE_H
#define _SERVICE_H

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
	void runServiceThread(map<int, int> *conn_table) 
	{
		serviceThread = thread(&Service::serviceProcess, this, conn_table);	
	}

	Service(int fd) 
	{
		connfd = fd;
	}
   
	~Service() 
	{
		serviceThread.join();
	}

private:

	int		connfd;

	thread	serviceThread;

	virtual void serviceProcess(map<int, int> *conn_table) 
	{
		throwError("[Service]: empty service process method");
	}

	/* use to delete the conn fd in table when the detect the client shut down */
	void processClientDown(int connfd, map<int, int> *conn_table)
	{
		cout << "\n[INFO]: Client<" << conn_table->at(connfd) << ">  has shut down" << endl;
		// delete the corresponding fd in table
		if(conn_table->find(connfd) != conn_table->end()) {
			conn_table->erase(conn_table->find(connfd));
		}
	}

	

};





#endif
