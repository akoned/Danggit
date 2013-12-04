/*
 * File:   dgt_http_body.h
 * Author: dyn
 *
 * Created on December 3, 2013, 3:47 PM
 */

#ifndef DGT_HTTP_BODY_H
#define	DGT_HTTP_BODY_H

#ifdef	__cplusplus
extern "C" {
#endif

	#include "dgt_config.h"
	#include "dgt_http.h"

	int dgt_http_body(struct http_request_t *req);

#ifdef	__cplusplus
}
#endif

#endif	/* DGT_HTTP_BODY_H */

