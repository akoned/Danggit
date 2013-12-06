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
#include "dgt_http_body.h"
#include "dgt_http_util.h"

#include "httpd_svc_log.h"
#include "dgt_sys_string.h"
#include "dgt_http_op.h"
#include "dgt_log.h"

#define MAXRESPHEADER	(4*1024)
#define MAXRESPBODY		(2*MAXRESPHEADER)
#define MAX_HTTP_RESP	(8*1024)
#define MAXHTTPREQPATH	(256)
#define HTTPINDEX		"/index.html"

int dgt_http_op_is_static_page(int type)
{
	int ret = 0;

	switch(type) {
		case HTTP_REQ_TYPE_IMGPNG:
		case HTTP_REQ_TYPE_IMGJPG:
		case HTTP_REQ_TYPE_CSS:
		case HTTP_REQ_TYPE_JS:
			ret = 1;
		default:
			break;
	}

	return ret;
}

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

int dgt_http_op_resp_io_send2(int fd, char *buf, size_t nSz)
{
	fd_set fds;
	struct timeval timeout;
	size_t nTotalSndSz = nSz, nSndSz;
	int ret = -1;
DGT_LOG_TRACE("START");
	while (nTotalSndSz) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		// set timeout
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		ret = dgt_os_select(fd + 1, NULL, &fds, NULL, &timeout);

		if (ret == -1) {
			HTTPDSVC_LOG_TRACE("error : header response!!!");
			break;
		} else if (ret) {
			if (FD_ISSET(fd, &fds)) {

				// check
				nSndSz = nTotalSndSz;
				if (nTotalSndSz > MAXRESPHEADER) {
					nSndSz = MAXRESPHEADER;
				}

				nSndSz = dgt_os_send(fd, buf, nSndSz, 0);
				nTotalSndSz -= nSndSz;
DGT_LOG_TRACE("nTotalSndSz.%d nSndSz.%d", nTotalSndSz, nSndSz);
			}
		} else {
			HTTPDSVC_LOG_TRACE("timeout : header response!!!");
		}
	}
DGT_LOG_TRACE("END");
	return ret;
}

int dgt_http_send_header(struct http_request_t *req)
{
	size_t nTotalSndSz = req->resp_header_sz;
	size_t nSndSz;
	char nRetry, bSndSts = -1;
	char *ptr = req->resp_header;
	int ret = -1;

	while (nTotalSndSz) {
		nSndSz = nTotalSndSz;
		if (nSndSz > DGT_HTTP_HEADER_IO_MAX) {
			nSndSz = DGT_HTTP_HEADER_IO_MAX;
		}
		nRetry = 0;
		bSndSts = -1;
		while (nRetry < 3) {
			ret = dgt_http_op_resp_io_send2(req->fd, ptr, nSndSz);
			if (!ret) {
				nRetry++;
			} else if (ret == 1) {
				ptr += nSndSz;
				bSndSts = 0;
				nTotalSndSz -= nSndSz;
				break;
			} else {
				HTTPDSVC_LOG_TRACE("Send header info error (%d)", errno);
				break;
			}
		}
	}

	return ret;
}

int dgt_http_send_static_body(struct http_request_t *req)
{
	char uri[DGT_MAX_URI];
	char *ptr = uri;
	int ret;
	FILE *pFile;
	size_t nTotalSndSz = req->resp_body_sz;
	size_t nSndSz;
	char nRetry, bSendSts = 0;

	memset(uri, 0x0, DGT_MAX_URI);

	// set URI
	ret = snprintf(ptr, DGT_MAX_URI, "%s", req->document_root);
	ptr += ret;
	ret = snprintf(ptr, DGT_MAX_URI - strlen(uri), "%s", req->uri);
	ptr += ret;

	ret = -1;

HTTPDSVC_LOG_TRACE("start : nTotalSndSz.%d", nTotalSndSz);
	pFile = dgt_os_fopen(uri, "rb");
	if (pFile) {
		while (nTotalSndSz) {

			req->status = DGT_HTTP_500_INTERNAL_ERR;

			if (bSendSts) {
				break;
			}

			nSndSz = nTotalSndSz;
			if (nSndSz > DGT_HTTP_BODY_IO_MAX) {
				nSndSz = DGT_HTTP_BODY_IO_MAX;
			}

			bSendSts = -1;
			nRetry = 0;
			ret = -1;	// set status fail

			nSndSz = dgt_os_fread(req->resp_body, 1, nSndSz, pFile);
			while (nRetry < 3) {
				ret = dgt_http_op_resp_io_send2(req->fd, req->resp_body, nSndSz);
HTTPDSVC_LOG_TRACE("send : ret.%d err.%d sz", ret, errno, nSndSz);
				if (!ret) {
					nRetry++;
				} else if (ret == 1) {
					ret = 0;
					bSendSts = 0;
					nTotalSndSz -= nSndSz;
					req->status = DGT_HTTP_200_OK;
					break;
				} else {
					break;
				}
			}
		}
		if (nRetry == 3) {
			req->status = DGT_HTTP_500_INTERNAL_ERR;
		}
		dgt_os_fclose(pFile);
	} else {
		req->status = DGT_HTTP_404_NOT_FOUND;
	}
HTTPDSVC_LOG_TRACE("end : nTotalSndSz.%d ret.%d", nTotalSndSz, ret);
	return ret;
}

int dgt_http_send_dynamic_body(struct http_request_t *req)
{
	size_t nTotalSndSz = req->resp_body_sz;
	size_t nSndSz;
	char nRetry, bSendSts = 0;
	int ret = -1;
	char *ptr = req->resp_body;

	req->status = DGT_HTTP_500_INTERNAL_ERR;

HTTPDSVC_LOG_TRACE("start : nTotalSndSz.%d", nTotalSndSz);
	while (nTotalSndSz) {

		bSendSts = -1;
		nRetry = 0;
		ret = -1;	// set status fail

		nSndSz = nTotalSndSz;
		if (nSndSz > MAX_HTTP_RESP_BODY) {
			nSndSz = MAX_HTTP_RESP_BODY;
		}

		while (nRetry < 3) {
			ret = dgt_http_op_resp_io_send2(req->fd, ptr, nSndSz);
HTTPDSVC_LOG_TRACE("send : ret.%d err.%d sz", ret, errno, nSndSz);
			if (!ret) {
				nRetry++;
			} else if (ret == 1) {
				ret = 0;
				bSendSts = 0;
				ptr += nSndSz;
				nTotalSndSz -= nSndSz;
				req->status = DGT_HTTP_200_OK;
				break;
			} else {
				break;
			}
		}
	}
HTTPDSVC_LOG_TRACE("end : nTotalSndSz.%d ret.%d", nTotalSndSz, ret);
	return ret;
}

/**
 *
 * @param req
 * @return
 */
int dgt_http_op_proc_resp_header(struct http_request_t *req)
{
	return dgt_http_header(req);
}

/**
 *
 * @param req
 * @return
 */
int dgt_http_op_proc_resp_body(struct http_request_t * req)
{
	int ret = -1;

	if (req == NULL) return -1;

	req->status = -1;

	// check if URI is index
	if (req->uri && (req->uri[0] == '/' || req->uri[0] == '\\')
			&& (strlen(req->uri) == 1)) {
		if (req->uri) dgt_sys_free(req->uri);
		req->uri = (char *)dgt_sys_calloc(strlen(HTTPINDEX) + 1);
		if (req->uri) {
			dgt_sys_strncat(req->uri, HTTPINDEX, strlen(HTTPINDEX));
		}
	}

	ret = dgt_http_body(req);

	return ret;
}

/**
 *
 * @param req
 * @return
 */
int dgt_http_op_resp_client(struct http_request_t * req)
{
	dgt_http_op_proc_resp_body(req);

	dgt_http_op_proc_resp_header(req);

	// response header
	dgt_http_send_header(req);
	// response body
	if (dgt_http_op_is_static_page(req->type)) {
		dgt_http_send_static_body(req);
	} else {
		dgt_http_send_dynamic_body(req);
	}
}

/**
 *
 * @param req
 * @return
 */
int dgt_http_op_resp(struct http_request_t * req)
{
	//printf("Request Header, conn(%d): \n%s", req->fd, req->request_header);

	/**< parse request header */
	dgt_http_util_InitReq(req);

	// process client response
	dgt_http_op_resp_client(req);

	// Release request
	dgt_httpd_util_ReleaseReq(req);

	return 0;
}
