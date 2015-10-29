#ifndef _MYSQL_H
#define _MYSQL_H

#include <mysql.h>
#include <iostream>

#include "unp.h"
#include "Exception.h"

class Mysql
{

public:

	/* here first connect to the mysql server to check whether the database exist,
		if not exist then create it, then clear the connection before connect to
		the target database
	*/
	void init(const char* database, const char* host="localhost", const char* name="chatroom", const char* pwd="753951")
	{
		// just connect to sql server
		connectToSql(NULL, host, name, pwd);
		createDatabase(database);
		freeConnPtr();
		// connect to the database required
		connectToSql(database, host, name, pwd);
	}	

	/* return the result ptr for caller to parse
	   the result ptr need to free by caller
	*/
	MYSQL_RES* query(const char* command) 
	{
		runCommand(command);
		res = mysql_store_result(connection);
		return res;
	}

	/* runCommand() method different from the query() is that runCommand() does 
		not have the return value, just to carry out the command
	*/
	void runCommand(const char* command)
	{
		if(!connection) {
			throwError("[mysql]: need to use init() connect to the database first");
		}
		if(mysql_query(connection, command) != 0) {
			cout << mysql_error(connection) << endl;
			throwError("[mysql]: mysql_query error");
		}		
	}
	
	/* return the row that query command calls
		the result ptr has been freed inside
		read the element from row: row[0], row[1], ... row[n]
	*/
	MYSQL_ROW selectRow(const char* command)
	{
		MYSQL_RES* res = query(command);
		MYSQL_ROW row = mysql_fetch_row(res);	
		freeResPtr();
		return row;	
	}	

	// similar to a iterator
	MYSQL_ROW getNextRow()
	{
		if(!res) throwError("[mysql]: result ptr is NULL");
		return mysql_fetch_row(res);
	}

	void freeResPtr()
	{
		if(res) mysql_free_result(res);
		res = 0;
	}
	
	void freeConnPtr()
	{
		if(connection) mysql_close(connection);
		connection = 0;
	}
	
	Mysql() 
	{
		res = 0;
		connection = 0;
	}

	~Mysql() 
	{
		freeResPtr();
		freeConnPtr();
	}



private:

	// use to record the consult result
	MYSQL_RES	*res;
	
	// use to track the sql
	MYSQL		*connection;

	void connectToSql(const char* database, const char* host, const char* name, const char* pwd)
	{
		if(connection) throwError("[mysql]: it need to clear the connection first");
		connection = mysql_init(NULL);
		if(!connection) {
			throwError("[mysql]: mysql_init error");
		}
		connection = mysql_real_connect(connection, host, name, pwd, database, 0, NULL, 0); 
		if(!connection) {
			cout << mysql_error(connection) << endl; 
			throwError("[mysql]: mysql_real_connect error");
		} else {
			if(database == NULL) cout << "[INFO]: connect to mysql server: success" << endl;
			else cout << "[INFO]: connect to database[" << database << "]: success" << endl;
		}
	}

	/* check the table whether exist or not,
		if not, then create the table with 	
	*/
	void createDatabase(const char* database)
	{
		char command[MAXLINE];		
		snprintf(command, MAXLINE, "CREATE DATABASE IF NOT EXISTS %s", database);
		if(unresCommand(command) != 0) {
			throwError("[mysql]: createDatabase error");
		}
	}

	virtual void createTable(const char* command) 
	{
		return runCommand(command);
	}

	/* carry out the command that does not have return value
		if success return 0, fail return -1
	*/
	int unresCommand(const char* command)
	{
		if(!connection) throwError("[mysql]: mysql is disconnect");	
	
		if(mysql_query(connection, command) != 0) {
			cout << mysql_error(connection) << endl;
			return -1;
		}		
		res = mysql_store_result(connection);
		return 0;
	}

};


#endif

