#ifndef _AUX_H
#define _AUX_H

#include <iostream>

#include "unp.h"
#include "Exception.h"

using namespace std;

/*------------------------------------------------------------------------
					S I G N A L
--------------------------------------------------------------------------
*/

void sig_chld(int signo)
{
	pid_t		pid;
	int			stat;

	while((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
#ifdef _DEBUG
		printf("***child %d of client terminated!***\n", pid);
#endif
	}
}

void sig_int(int signo)
{
	char input[MAXLINE];

	cout << endl;
	cout << "-------CTRL-C--------" << endl;	
	cout << "1> Go On             "	<< endl; 
	cout << "2> Back To Main Menu " << endl;
	cout << "3> Quit"				<< endl;
	cout << "---------------------" << endl;
	cout << "Enter your choice: ";
	fgets(input, MAXLINE, stdin);
	input[strlen(input)-1] = '\0';
	if(!strncmp(input, "1", strlen(input))) {
		return;
	} else if(!strcmp(input, "2")) {
		cout << "Back to main menu(future develop)" << endl;
		// here tmporary use exit 
		exit(0);
	} else if(!strcmp(input, "3")) {
		cout << "System is exiting..." << endl;
		exit(0);
	} else {
		cout << "Invalid input: please enter 1, 2 or 3" << endl;
	}	
}

/*------------------------------------------------------------------------
					N E T W O R K	
--------------------------------------------------------------------------
*/

int connectToServer(const char* ip, int port_num)
{
	int sockfd;
	struct sockaddr_in servaddr;		

	if((sockfd = socket(AF_INET, SOCK_STREAM,  0)) < 0) {
		throwError("[client]: socket build error");
	}
	bzero(&servaddr, sizeof(servaddr));	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port_num);
	if(inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0) {
		throwError("[client]: inet_pton() translate error");
	}	
	if(connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
		throwError("[client]: connect() error");	
	}
	
	return sockfd;
}


int initiateMonitor(int port_num, const char* interface = "INADDR_ANY")
{
	int					listenfd;
	const int			on = 1;
	struct sockaddr_in	servaddr;
		
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		throwError("[server]: socket build error");
	}
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		throwError("[server]: set socket option<SO_REUSEADDR> error");
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	if(strcmp(interface, "INADDR_ANY") == 0) {
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		if(inet_pton(AF_INET, interface, &servaddr.sin_addr) <= 0) {
			throwError("[server]: inet_pton() translate error");
		}				
	}
	servaddr.sin_port = htons(port_num);
	if(bind(listenfd, (SA*)&servaddr, sizeof(servaddr)) < 0) {
		throwError("[server]: bind error");
	}
	if(listen(listenfd, LISTENQ) < 0) {
		throwError("[server]: listen error");
	}

	return listenfd;
}

int acceptConnection(int listenfd)
{
	int				connfd;
	socklen_t		len;
	char			buff[MAXLINE];
	struct sockaddr_in cliaddr;

	len = sizeof(cliaddr);
	bzero(&cliaddr, sizeof(cliaddr));
	if((connfd = accept(listenfd, (SA*)&cliaddr, &len)) < 0) {
		throwError("[server]: accept error");
	}
	if(inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)) < 0) {
		throwError("[server]: inet_ntop error");
	}
	cout << "Connection from " << buff << ", port: " << ntohs(cliaddr.sin_port) << endl;

	return connfd;
}


/*------------------------------------------------------------------------
					F U N C T I O N	
--------------------------------------------------------------------------
*/

int scanfLine(char* buff)
{
	return scanf("%[^\n]%*c", buff);
}	

#endif
