/**
 * \file dgt_ntp.c
 * \desc Memory API
 *
 * Copyright(c) 2013 Neil Dy
 */
#include "dgt_config.h"
#include "dgt_os_io.h"
#include "dgt_ntp_op.h"
#include "dgt_ntp.h"

#include "dgt_log.h"

int dgt_ntp_init(struct ntp_config_t *hConfig)
{
	int ret;

	// Set configuration

	hConfig->domain = AF_UNIX;
	hConfig->type = SOCK_STREAM;
	hConfig->protocol = IPPROTO_IP;

	hConfig->max_conn = 10;

	hConfig->notify_http_req = dgt_http_req_add;
	hConfig->ntp_io = dgt_ntp_http_req_io;
	hConfig->ntp_accept = dgt_ntp_http_req_select;

	// Load system information into socket data structures

	memset(&hConfig->socketInfo_un, 0x0, sizeof(struct sockaddr_un));

	hConfig->socketInfo_un.sun_family = AF_UNIX;
	snprintf(hConfig->socketInfo_un.sun_path, 108, "/tmp/httpd_local");
	unlink("/tmp/httpd_local");

	// Create socket

	hConfig->fd = dgt_os_socket(	hConfig->domain,
									hConfig->type,
									hConfig->protocol);
	if (hConfig->fd < 0) {
		dgt_os_close(hConfig->fd);
		perror("ntp-socket-create");
		return -1;
	}


	// Bind the socket to a local socket address

	ret = dgt_os_bind(	hConfig->fd,
						(struct sockaddr *) &hConfig->socketInfo_un,
						sizeof(struct sockaddr_un));
	if (ret < 0) {
		dgt_os_close (hConfig->fd);
		perror("ntp-bind");
		return -1;
	}

	return 0;
}
#define MAXBUFF_REQ		(4*1024)
void * dgt_ntp_server(void *data)
{
	struct ntp_config_t *config = (struct ntp_config_t *)data;
	int new_conn, ret;
	//struct sockaddr_un remote;
	//size_t nRemoteSz = sizeof(remote);
	char tmp_buf[MAXBUFF_REQ+1];

	// Set MAX connection

	ret = dgt_os_listen(config->fd, config->max_conn);
	if (ret < 0) {
		dgt_os_close (config->fd);
		perror("ntp-listen");
		pthread_exit((void *)errno);
	}

	while (1) {

		if (config->ntp_accept) {
			new_conn = config->ntp_accept(config->fd);
		}

		DGT_LOG_TRACE("conn.%d err.%d\n", new_conn, errno);

		// read HTTP request
		if ((new_conn > 0) && (config->ntp_io)) {
			ret = config->ntp_io(new_conn, tmp_buf, MAXBUFF_REQ, DGT_NTP_IO_RD);
		}

		// notify HTTP request handler
		if ((ret > 0) && config->notify_http_req) {
			config->notify_http_req(new_conn,tmp_buf,strlen(tmp_buf));
		}
	}
}


int dgt_ntp_start()
{
	struct ntp_config_t *config = NULL;
	pthread_t thread_ntp = 0;
	int ret = -1;

	config = (struct ntp_config_t *)dgt_sys_alloc(sizeof(struct ntp_config_t));

	if (config)
		ret = dgt_ntp_init(config);

	if (!ret) {
		ret = pthread_create(&thread_ntp, NULL, dgt_ntp_server, (void *)config);
	}

	return ret;
}
