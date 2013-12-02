/**
 * \file dgt_httpd_op.c
 *
 * Copyright(c) 2013 Neil Dy
 */
#include "dgt_config.h"
#include "dgt_sys_mem.h"
#include "dgt_httpd.h"
#include "dgt_os_io.h"
#include "dgt_httpd_util.h"

#define MAXRESPHEADER	(4*1024)
#define MAXRESPBODY		(2*MAXRESPHEADER)
#define MAX_HTTP_RESP	(8*1024)
#define MAXHTTPREQPATH	(256)
#define HTTPINDEX		"/index.html"

/**
 *
 * @param req
 * @return
 */
int dgt_httpd_op_create_resp_header(struct http_request_t *req )
{
	return 0;
}

/**
 *
 * @param req
 * @return
 */
int dgt_httpd_op_create_resp_body(struct http_request_t * req)
{
	char httpReqPath[MAXHTTPREQPATH];
	FILE *pFile;
	long lSize;
	size_t nSz;
	char *pbody;
	int ret = -1;


	if (req == NULL) return -1;

	dgt_sys_strncat(httpReqPath, req->document_root,
							strlen(req->document_root));

	req->status = -1;

	// check if URI is index
	if (req->uri && (req->uri[0] == '/' || req->uri[0] == '\\')) {
		dgt_sys_strncat(httpReqPath, HTTPINDEX,
							strlen(HTTPINDEX));
	} else {
		dgt_sys_strncat(httpReqPath, req->uri,
							strlen(req->uri));
	}

	pFile = dgt_os_fopen(httpReqPath, "rb");
	if (pFile) {

		fseek(pFile, 0, SEEK_END);
		lSize = ftell(pFile);
		rewind(pFile);

		if (lSize > 0) {
			req->html_page = (char *)dgt_sys_alloc(lSize);
			if (req->html_page) {

				dgt_os_fread(req->html_page, 1, lSize, pFile);

				dgt_os_fclose(pFile);
			}
		}
	}

	if (req->status == -1) {
		pbody = req->resp_body;

		nSz = snprintf(pbody, (req->resp_body_sz - strlen(req->resp_body)), "<html>");
		pbody = pbody+nSz;

		nSz = snprintf(pbody, (req->resp_body_sz - strlen(req->resp_body)), "<head><title>Test Html</title></head>");
		pbody = pbody+nSz;

		nSz = snprintf(pbody, (req->resp_body_sz - strlen(req->resp_body)), "<body><h1>HTTP Not Found 404</h1></body>");
		pbody = pbody+nSz;

		nSz = snprintf(pbody, (req->resp_body_sz - strlen(req->resp_body)), "</html>");
		pbody = pbody+nSz;
	}

	return ret;
}

/**
 *
 * @param req
 * @return
 */
int dgt_httpd_op_resp_client(struct http_request_t * req)
{

}

/**
 *
 * @param req
 * @return
 */
int dgt_httpd_op_resp(struct http_request_t * req)
{
	int ret;

	printf("Request Header, conn(%d): \n%s", req->fd, req->request_header);

	/**< parse request header */
	ret = dgt_httpd_util_InitReq(req);

	if (!ret) {
		ret = dgt_httpd_op_create_resp_body(req);
	}

	if (!ret) {
		ret = dgt_httpd_op_create_resp_header(req);
	}

	if (!ret) {
		ret = dgt_httpd_op_resp_client(req);
	}

	return ret;
}


