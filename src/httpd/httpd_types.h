/*
 * File:   httpd_types.h
 * Author: dyn
 *
 * Created on November 11, 2013, 5:17 PM
 */

#ifndef HTTPD_TYPES_H
#define	HTTPD_TYPES_H

#ifdef	__cplusplus
extern "C" {
#endif

struct socket_t {
	int fd;	/**< socket file descriptor */
	int domain;
	int type;
	int protocol;
	struct sockaddr_in socketInfo_in;
	struct sockaddr_un socketInfo_un;
};

struct httpd_conf {
	struct socket_t net;
	struct socket_t loc;
	int max_conn;	/**< max connection */
	int port;

	/**< function operation */
	int (* httpd_req_proc)(int client_fd, struct httpd_conf *);
};

#ifdef	__cplusplus
}
#endif

#endif	/* HTTPD_TYPES_H */
