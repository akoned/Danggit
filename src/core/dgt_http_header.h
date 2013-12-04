/**
 * \file dgt_http_header.h
 *
 * Copyright(c) 2013 Neil Dy
 */

#ifndef DGT_HTTP_HEADERS_H
#define	DGT_HTTP_HEADERS_H

#ifdef	__cplusplus
extern "C" {
#endif

	#include "dgt_config.h"
	#include "dgt_http.h"

	#define DGT_MAX_URI	256

	enum {
		DGT_HTTP_200_OK			= 200,
		DGT_HTTP_403_FORBIDDEN	= 403,
		DGT_HTTP_404_NOT_FOUND	= 404,
		DGT_HTTP_500_INTERNAL_ERR,
	};

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

	int dgt_http_header(struct http_request_t * req);

#ifdef	__cplusplus
}
#endif

#endif	/* DGT_HTTP_HEADERS_H */
