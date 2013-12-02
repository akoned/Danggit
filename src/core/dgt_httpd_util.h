/*
 * File:   dgt_httpd_util.h
 * Author: dyn
 *
 * Created on November 25, 2013, 5:37 PM
 */

#ifndef DGT_HTTPD_UTIL_H
#define	DGT_HTTPD_UTIL_H

#ifdef	__cplusplus
extern "C" {
#endif

int dgt_httpd_util_InitReq(struct http_request_t * req);

enum {
	HTTP_REQ_TYPE_HTMLTXT	= 0,
	HTTP_REQ_TYPE_IMGPNG,
	HTTP_REQ_TYPE_IMGJPG,
	HTTP_REQ_TYPE_JS,		// Javascript
	HTTP_REQ_TYPE_CSS,		// CSS
};

enum {
	HTTP_METHOD_GET	= 0,
	HTTP_METHOD_SET,
	HTTP_METHOD_POST,
};

#ifdef	__cplusplus
}
#endif

#endif	/* DGT_HTTPD_UTIL_H */

