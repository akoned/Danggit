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

#include "list.h"

	struct header_field_t {
		char * ptr;	// address of header field value
		size_t sz;	// size of header field value
	};

	struct http_request_t {
		int fd;
		char *server_name;

		// HTTP server configuration
		char *document_root;

		// request parameter
		char * request_header;
		size_t request_header_buffer_sz;
		size_t request_header_sz;
		char * request_raw;
		size_t request_raw_sz;
		int method;
		int type;
		char *uri;

		// response parameter
		char *resp_header;
		size_t resp_header_sz;
		char *resp_body;
		char *html_page;
		size_t resp_body_buffer_sz;
		size_t resp_body_sz;
		int status;
	};

	struct http_request_entry {
		struct http_request_t req;
		struct list_head list;
	};

	struct http_config_t {
		struct list_head *req_list;
		char *web_root;

		int (*resp_cb)(struct http_request_t *);
	};

	int dgt_http_req_add(int fd, char *req_header, int sz);

#ifdef	__cplusplus
}
#endif

#endif	/* DGT_HTTP_REQUEST_H */

