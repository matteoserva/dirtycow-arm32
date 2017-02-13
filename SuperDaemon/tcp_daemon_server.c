#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "client.h"
#define PORT 11112
int tcp_daemon_create(int argc, char**argv)
{
	int  sockfd;
	int port = PORT;
	struct sockaddr_in my_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		return -1;
	}

	int one = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	memset(&my_addr,0,sizeof(my_addr));
	// set struct values
	my_addr.sin_family = AF_INET; // 2
	my_addr.sin_port = htons(port); // port number
	my_addr.sin_addr.s_addr = INADDR_ANY; // 0 fill with the local IP

	bind(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr));

	
	listen(sockfd, 1);
	
	return sockfd;
}



int tcp_client_create(int argc, char**argv)
{
	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1",&sa.sin_addr.s_addr);
	
	sa.sin_port = htons(PORT);
	
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		return -1;
	}
	
	if(connect(sockfd,(struct sockaddr*)&sa,sizeof(struct sockaddr_in)) < 0)
	{

		printf("connect failed \n");
		close(sockfd);
		return -1;
	}
	
	return sockfd;
	
	
	
}