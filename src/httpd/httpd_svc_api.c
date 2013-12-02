/**
 * \file httpd_main.c
 *
 * Copyright(c) 2013 Neil Dy
 */
#include "dgt_config.h"
#include "httpd_svc_api.h"


#define MAXHOSTNAME 256
#define MAXBUFFER	(1024*4)

int httpd_start_()
{
	int socketHandle;
	char sysHost[MAXHOSTNAME+1];
	struct hostent *hPtr;	//@orig: struct hostNamePtr
	int portNumber = 8081;
	struct sockaddr_in socketInfo, client_addr;
	int socketConnection;
	int sin_size;
	char buff[MAXBUFFER + 1];

	memset(&socketInfo, 0x0, sizeof(struct sockaddr_in));  // Clear structure memory

	/**< Initialize config settings */

	// create socket

	if ((socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0)
	{
		close (socketHandle);
		exit(EXIT_FAILURE);
	}

	// Get system information

	gethostname(sysHost, MAXHOSTNAME);	// Get the name of this computer

	if ((hPtr = gethostbyname(sysHost)) == NULL)
	{
		exit(EXIT_FAILURE);
	}

	// Load system information into socket data structures

	socketInfo.sin_family = AF_INET;
	socketInfo.sin_addr.s_addr = htonl(INADDR_ANY);
	socketInfo.sin_port = htons(portNumber);

	// Bind the socket to a local socket address

	if ( bind(socketHandle, (struct sockaddr *) &socketInfo, sizeof(struct sockaddr_in)) < 0)
	{
		close (socketHandle);
		perror("bind");
		exit(EXIT_FAILURE);
	}

	listen (socketHandle, 1);

	for (;;)
	{
		sin_size = sizeof(struct sockaddr_in);
		if ((socketConnection = accept(	socketHandle,
										(struct sockaddr *)&client_addr,
										&sin_size)) < 0)
		{
			close (socketHandle);
			exit(EXIT_FAILURE);
		}
		printf("I got a connection from (%s , %d)\n",
                   inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

	}
}