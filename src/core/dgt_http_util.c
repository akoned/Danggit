/**
 * \file dgt_httpd_util.c
 *
 * Copyright(c) 2013 Neil Dy
 */
#include "dgt_config.h"
#include "dgt_http.h"
#include "dgt_http_util.h"
#include "dgt_http_header.h"

#include "dgt_sys_string.h"

static size_t dgt_http_util_getReqUri(struct http_request_t *req, char **pValue)
{
	char * ptr = NULL;
	size_t nSz = 0;

	if (req == NULL || req->request_header == NULL) return -1;

	// search string GET
	ptr = dgt_sys_strstr(req->request_header, "GET");
	if (ptr) {
		ptr += 4;	// add offset GET length + space
		*pValue = ptr;
		while (ptr) {
			if (ptr[nSz] == 0x20) {
				break;
			}
			nSz++;
		}
	}

	return nSz;
}

static int dgt_httpd_util_getReqType(struct http_request_t *req)
{
	int http_req_type = HTTP_REQ_TYPE_HTMLTXT;
	char *ptr;

	if (req == NULL || req->request_header == NULL) return -1;

	// search dot for reference of extension
	ptr = dgt_sys_strstr(req->request_header, ".");
	if (ptr) {
		/**
		 * TODO
         */
		// get extention filename
		ptr += 1;
		if (!dgt_sys_strncmp(ptr, "png", 3)) {
			http_req_type = HTTP_REQ_TYPE_IMGPNG;
		} else if (!dgt_sys_strncmp(ptr, "jpg", 3)) {
			http_req_type = HTTP_REQ_TYPE_IMGJPG;
		}
	}

	return http_req_type;
}

static int dgt_http_util_getReqMethod(struct http_request_t *req)
{
	int http_req_method = HTTP_METHOD_GET;

	if (req == NULL || req->request_header == NULL) return -1;

	// search keyword SET in request header
	if (dgt_sys_strstr(req->request_header, "SET")) {
		http_req_method = HTTP_METHOD_SET;
	}

	return http_req_method;
}

int dgt_http_util_InitReq(struct http_request_t * req)
{
	size_t nSz;
	char * pBuf;

	// get URI
	nSz = dgt_http_util_getReqUri(req, &pBuf);
	if (nSz > 0) {
		req->uri = (char *)dgt_sys_calloc(nSz+1);
		if (req->uri) {
			dgt_sys_strcpy((const char *)pBuf, nSz, req->uri, (nSz + 1));

		}
	}

	// get request type (HTML, JS, CSS, IMAGE...)
	req->type = dgt_httpd_util_getReqType(req);

	// get Method (GET, SET)
	req->method = dgt_http_util_getReqMethod(req);

	// initialize buffer for response
	req->resp_header = (char *)dgt_sys_calloc(MAX_HTTP_RESP_HEADER);
	req->resp_header_buffer_sz = MAX_HTTP_RESP_HEADER;
	req->resp_body = (char *)dgt_sys_calloc(MAX_HTTP_RESP_BODY);
	req->resp_body_buffer_sz = MAX_HTTP_RESP_BODY;

	return 0;
}

int dgt_httpd_util_ReleaseReq(struct http_request_t * req)
{
	if (req->uri)
		dgt_sys_free(req->uri);

	if (req->resp_header)
		dgt_sys_free(req->resp_header);

	if (req->resp_body)
		dgt_sys_free(req->resp_body);
}