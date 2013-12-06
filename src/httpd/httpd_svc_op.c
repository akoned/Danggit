/**
 * \file http_service.c
 * \desc
 *
 * Copyright(c) 2013 Neil Dy
 */
#include "dgt_config.h"
#include "dgt_sys_mem.h"
#include "dgt_os_io.h"
#include "httpd_types.h"
#include "httpd_svc_op.h"

#include "httpd_svc_log.h"
#include "dgt_sys_string.h"
#include "dgt_log.h"

#define HTTPDSVCRESPMAX		(8*1024)
#define HTTPD_RESP_MAX		(4*1024)

/**
 *
 * @param fd	File descriptor
 * @param buf	buffer
 * @param nSz	Size of buffer
 * @param io	I/O mode (Read/Write)
 * @return
 *	-1: error, 0: timeout, 1: success
 */
static int httpd_svc_io(int fd, char *buf, size_t nBufSz, size_t *nIoSz,int io)
{
	fd_set fds;
	struct timeval timeout;
	int sts = -1;
DGT_LOG_TRACE("START");
	while (buf && nIoSz) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		// set timeout
		timeout.tv_sec = 60*5;
		timeout.tv_usec = 0;

		if (io == HTTPD_IO_RD) {
			sts = dgt_os_select(fd + 1, &fds, NULL, NULL, &timeout);
		} else {
			sts = dgt_os_select(fd + 1, NULL, &fds, NULL, &timeout);
		}

		if (sts == -1) {
			dgt_os_perror("<<httpd_svc_get_http_req>>");
			break;
		} else if (sts) {
			// data available
			if (FD_ISSET(fd, &fds)) {
				if (io == HTTPD_IO_RD) {
					*nIoSz = dgt_os_recv(fd, buf, nBufSz, 0);
					DGT_LOG_TRACE("rcvSz.%d", *nIoSz);
				} else {
					*nIoSz = dgt_os_send(fd, buf, nBufSz, 0);
				}
				break;
			}
		} else {
			printf("%s : Data timeout \n", __FUNCTION__);
			break;
		}
	}
DGT_LOG_TRACE("END");
	return sts;
}

size_t httpd_svc_get_resp_body_sz(char * source)
{
	char * ptr1, *ptr2;
	int tmp, sz;


	ptr1 = dgt_sys_strstr(source, "Content-Length");
	ptr1 = dgt_sys_strstr(ptr1, ":");

	ptr2 = dgt_sys_strstr(ptr1, "\\r\\n");

	tmp = (int )(ptr1 - ptr2);

	sz = atoi(ptr1+1);

	return (size_t) sz;
}

int httpd_svc_resp2(int net_fd, int loc_fd)
{
	fd_set rdfds, wrfds;
	struct timeval timeout_loc, timeout_net;
	int ret, nrdSz, nwrSz, bResp = 0;
	char resp[HTTPDSVCRESPMAX];

	// read response
	while (1) {
		// response complete
		if (bResp) break;

		timeout_loc.tv_sec = 1;
		timeout_loc.tv_usec = 0;

		FD_ZERO(&rdfds);
		FD_SET(loc_fd, &rdfds);

		ret = dgt_os_select(loc_fd+1, &rdfds, NULL, NULL, &timeout_loc);

		if (ret < 0) {

		} else if (ret == 1) {
			memset(resp, 0x0, HTTPDSVCRESPMAX);
			// read response
			nrdSz = dgt_os_read(loc_fd, resp, HTTPDSVCRESPMAX);
			printf("%s, %d : nrdSz.%d err.%d\n", __FUNCTION__, __LINE__, nrdSz, errno);

			if (nrdSz > 0) {

				while (1) {
					timeout_net.tv_sec = 1;
					timeout_net.tv_usec = 0;

					FD_ZERO(&wrfds);
					FD_SET(net_fd, &wrfds);

					ret = dgt_os_select(net_fd+1, NULL, &wrfds, NULL, &timeout_net);

					if (ret < 0) {
						break;
					} else if (ret == 1) {
						nwrSz = dgt_os_send(net_fd, resp, strlen(resp), 0);
						printf("%s, %d : sendsz.%d err.%d\n", __FUNCTION__, __LINE__, nwrSz, errno);
						bResp = 1;
						break;
					} else {
						// timeout
					}
				}
			}
		} else {
			// timeout occur
		}
	}
}

#define HTTPD_SVC_RETRY	3
static int httpd_svc_resp(int client_fd, struct httpd_conf *pConfig)
{
	char work_buff[HTTPD_RESP_MAX];
	char *pWork_buff = work_buff;
	size_t nIoSz = 0, nRespBodySz = 0;
	int sts;
	int retry = 0;


	if (!(pConfig)) {
		return -1;
	}

	memset(work_buff, 0x0, HTTPD_RESP_MAX);
HTTPDSVC_LOG_TRACE("Start : Send Response Header");
	// send response header
	while (retry < HTTPD_SVC_RETRY) {
		sts = httpd_svc_io(	pConfig->loc.fd, work_buff, HTTPD_RESP_MAX,
								&nIoSz, HTTPD_IO_RD);

		if (!sts) {
			retry++;
		} else if (sts) {
			HTTPDSVC_LOG_TRACE("[from core] rcvSz.%d err.%d\n", nIoSz, errno);
			// response to client
			httpd_svc_io(	client_fd, work_buff,
							strlen (work_buff), &nIoSz, HTTPD_IO_WR);
			HTTPDSVC_LOG_TRACE("[from client] nIoSz.%d err.%d actual.%d\n",
									nIoSz, errno, strlen (work_buff));
			nIoSz = 0;
			break;
		} else {
			break;
		}
	}
HTTPDSVC_LOG_TRACE("End : Send Response Header\n");
	retry = 0;
	nRespBodySz = httpd_svc_get_resp_body_sz(work_buff);
HTTPDSVC_LOG_TRACE("Start : Send Response body (%d)", nRespBodySz);
	// read response
	while (nRespBodySz) {
		memset(work_buff, 0x0, HTTPD_RESP_MAX);
		sts = httpd_svc_io(	pConfig->loc.fd, work_buff,
							HTTPD_RESP_MAX, &nIoSz, HTTPD_IO_RD);

		if (!sts) {
			retry++;
		} else if (sts) {
			HTTPDSVC_LOG_TRACE("[from core] rcvSz.%d err.%d\n", nIoSz, errno);
			// response to client
			sts = httpd_svc_io(	client_fd, work_buff,
									strlen (work_buff), &nIoSz, HTTPD_IO_WR);
			HTTPDSVC_LOG_TRACE("[from client] sndSz.%d err.%d\n", nIoSz, errno);
			memset(work_buff, 0x0, nIoSz);
			retry = 0;
			nRespBodySz -= nIoSz;
		} else {
			break;
		}
	}
HTTPDSVC_LOG_TRACE("End : Send Response body (%d)\n", nRespBodySz);

	dgt_os_close(pConfig->loc.fd);
}

int httpd_svc_req_proc(int client_fd, struct httpd_conf * config)
{
	char work_buff[HTTPD_RESP_MAX];
	size_t nIoSz;
	int ret;
	int sts;

	// read HTTP request
	sts = httpd_svc_io(client_fd, work_buff, HTTPD_RESP_MAX, &nIoSz, HTTPD_IO_RD);
	HTTPDSVC_LOG_TRACE("http-req-sz.%d err.%d\n", nIoSz, errno);

	if (sts) {

		// create socket
		config->loc.fd = dgt_os_socket(	config->loc.domain,
										config->loc.type,
										config->loc.protocol);

		// connect to ntp
		ret = dgt_os_connect(	config->loc.fd,
								(struct sockaddr *)&config->loc.socketInfo_un,
								SUN_LEN(&config->loc.socketInfo_un));

		if (!ret) {

			HTTPDSVC_LOG_TRACE("<<connect>> SUCCESS");

			nIoSz = httpd_svc_io(	config->loc.fd,
									work_buff,
									strlen(work_buff),
									&nIoSz,
									HTTPD_IO_WR);
			HTTPDSVC_LOG_TRACE("notify-ntp-sz.%d err.%d\n", nIoSz, errno);

			httpd_svc_resp(client_fd, config);
		} else {
			HTTPDSVC_LOG_TRACE("<<connect>> FAIL, err.%d", errno);
		}
	}
}