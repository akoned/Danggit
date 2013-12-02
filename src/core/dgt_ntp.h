/**
 * \file dgt_ntp.h
 * \desc Network transfer protocol
 *
 * Copyright(c) 2013 Neil Dy
 */
#ifndef DGT_NTP_H
#define	DGT_NTP_H

#ifdef	__cplusplus
extern "C" {
#endif

	#include "dgt_config.h"
	#include "dgt_http.h"

	struct ntp_config_t {
		int fd;	/**< socket file descriptor */
		int domain;
		int type;
		int protocol;
		int max_conn;
		struct sockaddr_un socketInfo_un;

		// ntp operation
		int (*ntp_io)(int,char *,size_t, int);
		int (*ntp_accept)(int);

		int (*notify_http_req)(int,char *,size_t);
	};

#define DGT_NTP_IO_RD	1
#define DGT_NTP_IO_WR	2

	int dgt_ntp_start();

#ifdef	__cplusplus
}
#endif

#endif	/* DGT_NTP_H */

