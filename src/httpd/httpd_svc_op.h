/*
 * File:   httpd_svc_op.h
 * Author: dyn
 *
 * Created on November 19, 2013, 10:57 AM
 */

#ifndef HTTPD_SVC_OP_H
#define	HTTPD_SVC_OP_H

#ifdef	__cplusplus
extern "C" {
#endif

#define HTTPD_IO_RD		1
#define HTTPD_IO_WR		2

int httpd_svc_req_proc(int client_fd, struct httpd_conf *);

#ifdef	__cplusplus
}
#endif

#endif	/* HTTPD_SVC_OP_H */

