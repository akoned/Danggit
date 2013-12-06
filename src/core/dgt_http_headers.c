/**
 * \file dgt_httpd_header.c
 *
 * Copyright(c) 2013 Neil Dy
 */
#include "dgt_http_header.h"
#include "dgt_log.h"

void dgt_http_header_set_status(struct http_request_t * req)
{
	size_t nSz = strlen(req->resp_header);
	char * buff = req->resp_header + nSz;

	switch(req->status) {
		case DGT_HTTP_200_OK:
			snprintf(buff, (req->resp_header_buffer_sz - nSz),
								"HTTP/%s\r\n", "1.1 200 OK");
		case DGT_HTTP_404_NOT_FOUND:
			snprintf(buff, (req->resp_header_buffer_sz - nSz),
								"HTTP/%s\r\n", "1.1 404 Not Found");
			break;
		case DGT_HTTP_500_INTERNAL_ERR:
			snprintf(buff, (req->resp_header_buffer_sz - nSz),
								"HTTP/%s\r\n", "1.1 500 Internal Error");
			break;
		default :
			snprintf(buff, (req->resp_header_buffer_sz - nSz),
								"HTTP/%s\r\n", "1.1 200 OK");
	}
}

void dgt_http_header_set_contentType(struct http_request_t * req)
{
	size_t nSz = strlen(req->resp_header);
	char * buff = req->resp_header + nSz;

	switch(req->type) {
		case HTTP_REQ_TYPE_IMGPNG:
			snprintf(buff, (req->resp_header_buffer_sz - nSz),
								"Content-Type: %s\r\n", "image/png");
			break;
		case HTTP_REQ_TYPE_IMGJPG:
			snprintf(buff, (req->resp_header_buffer_sz - nSz),
								"Content-Type: %s\r\n", "image/jpg");
			break;
		case HTTP_REQ_TYPE_JS:
			snprintf(buff, (req->resp_header_buffer_sz - nSz),
								"Content-Type: %s\r\n", "text/javascript");
			break;
		case HTTP_REQ_TYPE_CSS:
			snprintf(buff, (req->resp_header_buffer_sz - nSz),
								"Content-Type: %s\r\n", "text/css");
			break;
		default:
			snprintf(buff, (req->resp_header_buffer_sz - nSz),
								"Content-Type: %s\r\n", "text/html");
	}
}

void dgt_http_header_set_connection(struct http_request_t * req)
{
	size_t nSz = strlen(req->resp_header);
	char * buff = req->resp_header + nSz;

	snprintf(buff, (req->resp_header_buffer_sz - nSz),
						"Connection: %s\r\n", "Keep-Alive");

}

void dgt_http_header_set_date(struct http_request_t * req)
{
	size_t nSz = strlen(req->resp_header);
	char * buff = req->resp_header + nSz;

	snprintf(buff, (req->resp_header_buffer_sz - nSz),
						"Date: %s\r\n", "Tue, 15 Nov 1994 08:12:31 GMT");
}

void dgt_http_header_set_server(struct http_request_t * req)
{
	size_t nSz = strlen(req->resp_header);
	char * buff = req->resp_header + nSz;

	snprintf(buff, (req->resp_header_buffer_sz - nSz),
						"Server: %s\r\n", req->server_name);
}

void dgt_http_header_set_contentLength(struct http_request_t * req)
{
	size_t nSz = strlen(req->resp_header);
	char * buff = req->resp_header + nSz;

	snprintf(buff, (req->resp_header_buffer_sz - nSz),
				"Content-Length: %d\r\n", req->resp_body_sz);
}

/**
 * Create response headers
 * @param req
 * @return
 */
int dgt_http_header(struct http_request_t * req)
{
	size_t nSz;
	char *buff;

	// set status
	dgt_http_header_set_status(req);
	// set content type
	dgt_http_header_set_contentType(req);
	// set content length
	dgt_http_header_set_contentLength(req);
	//
	dgt_http_header_set_server(req);
	//
	dgt_http_header_set_date(req);

	nSz = strlen (req->resp_header);
	buff = req->resp_header + nSz;

	snprintf(buff, (req->resp_header_buffer_sz - nSz), "\r\n");

	req->resp_header_sz = strlen (req->resp_header);
	DGT_LOG_TRACE("resp_header_sz.%d", req->resp_header_sz);
	return 0;
}