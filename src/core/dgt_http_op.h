/*
 * File:   dgt_httpd_op.h
 * Author: dyn
 *
 * Created on November 18, 2013, 6:29 PM
 */

#ifndef DGT_HTTPD_OP_H
#define	DGT_HTTPD_OP_H

#ifdef	__cplusplus
extern "C" {
#endif

#define DGT_HTTP_BODY_IO_MAX		(8*1024)
#define DGT_HTTP_HEADER_IO_MAX		(4*1024)

int dgt_http_op_resp(struct http_request_t * req);

#ifdef	__cplusplus
}
#endif

#endif	/* DGT_HTTPD_OP_H */

