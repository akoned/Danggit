/**
 * \file dgt_http_request.c
 * \desc
 *
 * Copyright(c) 2013 Neil Dy
 */
#include "list.h"

#include "dgt_config.h"
#include "dgt_sys_mem.h"
#include "dgt_http_request.h"

static struct list_head http_req_list_head;


dgt_http_req_init()
{
	INIT_LIST_HEAD(&http_req_list_head);
}

int dgt_http_req_add(int fd, char *req_header, int sz)
{
	struct http_request_entry * pHttpReq = NULL;

	pHttpReq = (struct http_request_entry *)dgt_sys_alloc(sizeof (struct http_request_entry));
	if (pHttpReq) {
		pHttpReq->req.request_header = (char *)dgt_sys_calloc(sz+1);
		if (pHttpReq->req.request_header) {

			pHttpReq->req.fd = fd;
			memcpy(pHttpReq->req.request_header, req_header, sz);

			list_add(&pHttpReq->list, &http_req_list_head);

		} else {
			// free allocated HTTP request
			dgt_sys_free(pHttpReq);
		}
	}
}