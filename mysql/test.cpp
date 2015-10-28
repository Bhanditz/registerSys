#include "Mysql.h"
#include "unp.h"

int main()
{
	Mysql mysql;

	mysql.init("foo");

	// run this kind of command, the return result ptr will be NULL
	char buff[MAXLINE];
	strcpy(buff, "CREATE TABLE Persons ( Id_P int NOT NULL PRIMARY KEY, LastName varchar(255) NOT NULL, FirstName varchar(255), Address varchar(255), City varchar(255) )");
	mysql.runCommand(buff);		



	return 0;
}
