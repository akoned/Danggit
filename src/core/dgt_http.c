/**
 * \file dgt_http.c
 *
 * Copyright(c) 2013 Neil Dy
 */
#include "dgt_config.h"
#include "dgt_sys_mem.h"
#include "dgt_sys_string.h"
#include "dgt_http.h"
#include "dgt_http_op.h"

#define DEFAULT_WEBROOT	"./www"

static struct list_head http_req_list_head;

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

int dgt_http_req_init(struct http_config_t *hconfig)
{
	int nSz;

	hconfig->req_list = &http_req_list_head;

	INIT_LIST_HEAD(hconfig->req_list);

	// set Response callback function
	hconfig->resp_cb = dgt_http_op_resp;

	// set WEB_ROOT
	nSz = strlen(DEFAULT_WEBROOT);
	hconfig->web_root = NULL;
	hconfig->web_root = (char *)dgt_sys_calloc(nSz+1);
	if (hconfig->web_root) {
		dgt_sys_strcpy(	DEFAULT_WEBROOT, nSz, hconfig->web_root, nSz+1);
	}

	return 0;
}

int dgt_http_req_handler(void *data)
{
	struct http_config_t *config = (struct http_config_t *)data;
	int ret = 0;
	size_t nSz;
	struct list_head *pos = NULL;
	struct http_request_entry *req_ent = NULL;

	for (;;) {
		usleep(1000*1000);

		if (!list_empty(config->req_list)) {
			/* get first request (FIFO) */
			list_for_each(pos, config->req_list) {
				req_ent = list_entry(pos, struct http_request_entry, list);
				break;
			}
			if (pos) list_del(pos);

			if (config->resp_cb && req_ent != NULL) {

				/*
				 * set HTTPD configuration
				 */

				// set EWEBROOT
				nSz = strlen(config->web_root);
				if (nSz > 0) {
					req_ent->req.document_root = (char *)dgt_sys_calloc(nSz + 1);

					dgt_sys_strcpy(	config->web_root, nSz,
									req_ent->req.document_root, nSz + 1);
				}

				config->resp_cb(&req_ent->req);
			}
			printf("list is not empty!!!!\n");
		}
	}

	pthread_exit((void *)ret);
}

int dgt_http_req_start()
{
	struct http_config_t *hconfig = NULL;
	int ret = -1;
	pthread_t thread_http_handler;

	hconfig = (struct http_config_t *)dgt_sys_calloc(sizeof(struct http_config_t));

	if (hconfig) {
		ret = dgt_http_req_init(hconfig);
	}

	if(!ret) {
		ret = pthread_create(	&thread_http_handler,
								NULL,
								dgt_http_req_handler,
								(void *)hconfig);
	}

	pthread_join(thread_http_handler, NULL);

	return 0;
}