#ifndef _GETPWD_H
#define _GETPWD_H

#include <iostream>
#include <termios.h>  
#include <unistd.h>  

#include "unp.h"
#include "Exception.h"

using namespace std;

#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL)  

void setDispMode(int, int);

int readPassword(char* password, size_t len)  
{  
	int c = 0; /* getchar() returns int */  
	int n = 0;  

	bzero(password, len);	
	setDispMode(STDIN_FILENO, 0);
	while(c != '\n') {
		c = getchar();
		password[n++] = c;
		cout << "*";
		if(n == (int)len-1) {
			throwError("[LOGIN]: password is too long");
		}
	}
	password[n] = '\0';  
	password[n-1] = '\0';

	setDispMode(STDIN_FILENO, 1);
	cout << endl;
	return n;  
}  

void setDispMode(int fd, int option)  
{  
	int err;  
	struct termios term;  

	if(tcgetattr(fd, &term) == -1){  
		throwError("[LOGIN]: Cannot get the attribution of the terminal");  
	}  
	if(option)	term.c_lflag |= ECHOFLAGS;  
	else		term.c_lflag &= ~ECHOFLAGS;  

	err = tcsetattr(fd, TCSAFLUSH, &term);  
	if(err == -1 && err == EINTR){  
		throwError("[LOGIN]: Cannot set the attribution of the terminal");  
	}  
}  



#endif
