/**
 * \file dgt_httpd_op.c
 *
 * Copyright(c) 2013 Neil Dy
 */
#include "dgt_config.h"
#include "dgt_sys_mem.h"
#include "dgt_os_io.h"

#include "dgt_http.h"
#include "dgt_http_header.h"
#include "dgt_http_util.h"

#include "httpd_svc_log.h"

#define MAXRESPHEADER	(4*1024)
#define MAXRESPBODY		(2*MAXRESPHEADER)
#define MAX_HTTP_RESP	(8*1024)
#define MAXHTTPREQPATH	(256)
#define HTTPINDEX		"/index.html"

int dgt_httpd_op_resp_io_send(struct http_request_t *req)
{
	fd_set fds;
	struct timeval timeout;
	size_t nTotalSndSz = req->request_header_sz, nSndSz;
	int ret;
	char *buf = req->resp_header;

	while (nTotalSndSz) {
		FD_ZERO(&fds);
		FD_SET(req->fd, &fds);

		// set timeout
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		ret = dgt_os_select(req->fd + 1, NULL, &fds, NULL, &timeout);

		if (ret == -1) {
			HTTPDSVC_LOG_TRACE("error : header response!!!");
			break;
		} else if (ret) {
			if (FD_ISSET(req->fd, &fds)) {

				// check
				nSndSz = nTotalSndSz;
				if (nTotalSndSz > MAXRESPBODY) {
					nSndSz = MAXRESPBODY;
				}

				nTotalSndSz -= dgt_os_send(req->fd, buf, nSndSz, 0);
			}
		} else {
			HTTPDSVC_LOG_TRACE("timeout : header response!!!");
		}
	}

	nTotalSndSz = req->resp_body_sz;
	while (nTotalSndSz) {
		FD_ZERO(&fds);
		FD_SET(req->fd, &fds);

		// set timeout
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		ret = dgt_os_select(req->fd + 1, NULL, &fds, NULL, &timeout);

		if (ret == -1) {
			HTTPDSVC_LOG_TRACE("error : header response!!!");
			break;
		} else if (ret) {
			if (FD_ISSET(req->fd, &fds)) {

				// check
				nSndSz = nTotalSndSz;
				if (nTotalSndSz > MAXRESPHEADER) {
					nSndSz = MAXRESPHEADER;
				}

				nTotalSndSz -= dgt_os_send(req->fd, buf, nSndSz, 0);
			}
		} else {
			HTTPDSVC_LOG_TRACE("timeout : header response!!!");
		}
	}
}

/**
 *
 * @param req
 * @return
 */
int dgt_httpd_op_create_resp_header(struct http_request_t *req)
{
	return dgt_http_header(req);
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
		req->status = DGT_HTTP_404_NOT_FOUND;
		pbody = req->resp_body;

		nSz = snprintf(pbody, (req->resp_body_buffer_sz - strlen(req->resp_body)), "<html>");
		pbody = pbody+nSz;

		nSz = snprintf(pbody, (req->resp_body_buffer_sz - strlen(req->resp_body)), "<head><title>Test Html</title></head>");
		pbody = pbody+nSz;

		nSz = snprintf(pbody, (req->resp_body_buffer_sz - strlen(req->resp_body)), "<body><h1>HTTP Not Found 404</h1></body>");
		pbody = pbody+nSz;

		nSz = snprintf(pbody, (req->resp_body_buffer_sz - strlen(req->resp_body)), "</html>");
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
	dgt_httpd_op_create_resp_body(req);

	dgt_httpd_op_create_resp_header(req);
}

/**
 *
 * @param req
 * @return
 */
int dgt_httpd_op_resp(struct http_request_t * req)
{
	printf("Request Header, conn(%d): \n%s", req->fd, req->request_header);

	/**< parse request header */
	dgt_httpd_util_InitReq(req);

	// process client response
	dgt_httpd_op_resp_client(req);

	// Release request
	dgt_httpd_util_ReleaseReq(req);

	return 0;
}
