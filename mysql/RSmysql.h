
/* class RSmysql is inherited from class Mysql,
	which has the specific method to build up the API of register system
*/

#ifndef _RSMYSQL_H
#define _RSMYSQL_H

#include <stdarg.h>

#include "Mysql.h"


class RSmysql : public Mysql
{

public:

	// Users: UserName, PassWord
	void createUserTable()
	{
		createTable("Users", 2, "UserName", "varchar(255)", "PassWord", "varchar(255)");
	}

	void addNewUser(const char* user_name, const char* password)	
	{
		char	command[MAXLINE];

		if(isUserExist(user_name)) {
			throwError("[mysql]: the input username has been existed");
		}
		if(strlen(password) < 6) {
			throwError("[new user]: password is set too short");
		}
		snprintf(command, MAXLINE, "INSERT INTO Users (UserName, PassWord) Values ('%s', '%s')", user_name, password);
		runCommand(command);
	}  

	// use for check the type-in username and pwd when user log in
	bool isUserValid(const char* name, const char* pwd) 
	{
		if(!isUserExist(name)) {
			throwError("[log in]: the target user is not existed");
		}	
		// if user name is existed
		if(!isPwdValid(name, pwd)) return false;
		return true;		
	}
	
	bool isUserExist(const char* user_name)
	{
		MYSQL_ROW row;
		char command[MAXLINE];
		
		snprintf(command, MAXLINE, "SELECT UserName FROM Users WHERE UserName='%s'", user_name);
		row = selectRow(command); 
		if(row == NULL) return false;
		else return true;
	}

	


private:

	/* this method is to create a table in the database has connected to,
		and after type in the number of parameters, then type in the the name of
		parameters and its corresponding premitive type
		(Id parameter is added in default) 
	*/
	void createTable(const char* table_name, int pair_num, ...)
	{
		va_list		val;	
		char		*para_name, *para_type;
		char		buff[MAXLINE];
		
		if(pair_num < 1) throwError("[mysql]: table must have at least 1 column");

		va_start(val, pair_num);
		snprintf(buff, MAXLINE, "CREATE TABLE IF NOT EXISTS %s ( ", table_name);
		strcat(buff, " Id int NOT NULL PRIMARY KEY AUTO_INCREMENT");	

		while(pair_num) {
			para_name = va_arg(val, char*);	
			para_type = va_arg(val, char*);
			strcat(buff, ", ");
			strcat(buff, para_name); 
			strcat(buff, " ");	
			strcat(buff, para_type);
			strcat(buff, " NOT NULL ");
			pair_num--;
		}	
		strcat(buff, ")");
		va_end(val);

		runCommand(buff);
	}	

	bool isPwdValid(const char* user_name, const char* password)
	{
		char		command[MAXLINE];
		MYSQL_ROW	row;

		snprintf(command, MAXLINE, "SELECT * FROM Users WHERE UserName='%s'", user_name);
		row = selectRow(command);	
		if(row == NULL) throwError("[log in]: getUserPwd error");	
		if(!strcmp(row[2], password)) return true;
		return false;
	}

};


#endif
