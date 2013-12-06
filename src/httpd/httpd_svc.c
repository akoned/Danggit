/**
 * \file http_service.c
 * \desc
 *
 * Copyright(c) 2013 Neil Dy
 */
#include "dgt_config.h"
#include "dgt_os_io.h"
#include "dgt_sys_mem.h"

#include "list.h";

#include "httpd_types.h"
#include "httpd_svc_op.h"

#include "httpd_svc_log.h"

#define MAXBUFF (8*1024)
#define MAX_HTML_BODY	(4*1024)

#define LOCAL_SOCKET_PATH	"/tmp/httpd_local"

int httpd_init(struct httpd_conf *hConfig)
{
	struct socket_t * net = &hConfig->net;
	struct socket_t * loc = &hConfig->loc;
	struct sockaddr_in * pSockaddr = NULL;
	int ret;

	// Load configuration

	net->domain = AF_INET;
	net->type = SOCK_STREAM;
	net->protocol = IPPROTO_IP;

	loc->domain = AF_UNIX;
	loc->type = SOCK_STREAM;
	loc->protocol = IPPROTO_IP;

	hConfig->max_conn = 10;
	hConfig->port = 8085;

	// HTTPD I/O
	hConfig->httpd_req_proc = httpd_svc_req_proc;

	// Create socket

	net->fd = socket(	net->domain,
						net->type,
						net->protocol);

	loc->fd = socket(	loc->domain,
						loc->type,
						loc->protocol);

	if((net->fd < 0) || (loc->fd < 0)) {
		close (net->fd);
		close (loc->fd);
		perror("create socket");
		return -1;
	}

	HTTPDSVC_LOG_SYS("net-fd(%d) loc-fd(%d)\n", net->fd, loc->fd);

	// Load system information into socket data structures

	memset(&net->socketInfo_in, 0x0, sizeof(struct sockaddr_in));  // Clear structure memory
	memset(&loc->socketInfo_un, 0x0, sizeof(struct sockaddr_un));

	net->socketInfo_in.sin_family = AF_INET;
	net->socketInfo_in.sin_addr.s_addr = htonl(INADDR_ANY);
	net->socketInfo_in.sin_port = htons(hConfig->port);

	loc->socketInfo_un.sun_family = AF_UNIX;
	snprintf(loc->socketInfo_un.sun_path, 108, LOCAL_SOCKET_PATH);

	// Bind the socket to a local socket address
	ret = bind(	net->fd,
				(struct sockaddr *) &net->socketInfo_in,
				sizeof(struct sockaddr_in));
	if (ret < 0) {
		close (net->fd);
		close (loc->fd);
		perror("bind-net");
		return -1;
	}

	return 0;
}

void * httpd_net(void *data)
{
	struct httpd_conf *config = (struct httpd_conf *)data;
	struct sockaddr_in client_addr;
	int ret;
	int sin_size;
	int clientConn;

	ret = listen(config->net.fd, config->max_conn);

	while (1) {
		sin_size = sizeof(struct sockaddr_in);
		if ((clientConn = dgt_os_accept(	config->net.fd,
											(struct sockaddr *)&client_addr,
											&sin_size)) < 0)
		{
			dgt_os_close (config->net.fd);
			exit(EXIT_FAILURE);
		}

		HTTPDSVC_LOG_TRACE("I got a connection from (%s , %d) client id (%d)\n",
								inet_ntoa(client_addr.sin_addr),
								ntohs(client_addr.sin_port),
								clientConn);


		if(config->httpd_req_proc) {

			// process HTTP Request
			config->httpd_req_proc(clientConn, config);

			dgt_os_close(clientConn);
		}
	}

	pthread_exit((void *)ret);
}


int httpd_start()
{
	struct httpd_conf *conf;
	pthread_t thread_net = 0;
	int ret = -1;

	// allocate socket handles
	conf = (struct httpd_conf *)dgt_sys_alloc(sizeof(struct httpd_conf));

	// initialize socket
	if (conf != NULL)
		ret = httpd_init(conf);

	if (!ret) {
		ret = pthread_create(&thread_net, NULL, httpd_net, (void *)conf);
	} else {
		HTTPDSVC_LOG_SYS("HTTPD Service initialize fail(%d)\n",ret);
	}

	//pthread_join(thread_net, NULL);

	return ret;
}
