/**
 * \file dgt_httpd_body.c
 *
 * Copyright(c) 2013 Neil Dy
 */
#include "dgt_config.h"
#include "dgt_os_io.h"
#include "dgt_sys_string.h"

#include "dgt_http_header.h"
#include "dgt_http_body.h"
#include "dgt_sys_mem.h"

int dgt_http_prep_static_body(struct http_request_t *req)
{
	char url[DGT_MAX_URI];
	FILE *pFile;
	long lSize;
	int ret = -1;

	memset (url, 0x0, DGT_MAX_URI);

	dgt_sys_strncat(url, req->document_root, strlen(req->document_root));
	dgt_sys_strncat(url, req->uri, strlen(req->uri));

	pFile = dgt_os_fopen(url, "rb");
	if (pFile) {
		fseek(pFile, 0, SEEK_END);
		lSize = ftell(pFile);
		rewind(pFile);

		req->resp_body_sz = lSize;
		ret = 0;

		dgt_os_fclose(pFile);
	}

	return ret;
}

int dgt_http_prep_dynamic_body(struct http_request_t *req)
{
	char url[256];
	FILE *pFile;
	long lSize;
	int ret = -1;

	memset(url, 0x0, 256);

	dgt_sys_strncat(url, req->document_root, strlen(req->document_root));
	dgt_sys_strncat(url, req->uri, strlen(req->uri));

	pFile = dgt_os_fopen(url, "rb");
	if (pFile) {
		fseek(pFile, 0, SEEK_END);
		lSize = ftell(pFile);
		rewind(pFile);

		req->resp_body_sz = lSize;
		ret = 0;

		// temporary
		if (req->resp_body) dgt_sys_free(req->resp_body);

		req->resp_body = (char *) dgt_sys_alloc(req->resp_body_sz);

		dgt_os_fread(req->resp_body, 1, req->resp_body_sz, pFile);
		// end temporary

		dgt_os_fclose(pFile);
	}

	return ret;
}

int dgt_http_body(struct http_request_t *req)
{
	int ret = -1;

	if (req == NULL) return ret;

	switch(req->type) {
		case HTTP_REQ_TYPE_IMGPNG:
		case HTTP_REQ_TYPE_IMGJPG:
		case HTTP_REQ_TYPE_JS:
		case HTTP_REQ_TYPE_CSS:
			ret = dgt_http_prep_static_body(req);
			break;
		case HTTP_REQ_TYPE_HTMLTXT:
		default:
			ret = dgt_http_prep_dynamic_body(req);
			break;
	}
}