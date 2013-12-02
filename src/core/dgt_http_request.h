/*
 * File:   dgt_http_request.h
 * Author: dyn
 *
 * Created on November 16, 2013, 8:56 PM
 */

#ifndef DGT_HTTP_REQUEST_H
#define	DGT_HTTP_REQUEST_H

#ifdef	__cplusplus
extern "C" {
#endif

	struct http_request_t {
		int fd;
		char * request_header;
		unsigned int request_header_sz;
		unsigned int method_offset;
	};

	struct http_request_entry {
		struct http_request_t req;
		struct list_head list;
	};

#ifdef	__cplusplus
}
#endif

#endif	/* DGT_HTTP_REQUEST_H */

