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

#define HTTPDSVCRESPMAX		(8*1024)
#define HTTPD_RESP_MAX		(4*1024)

/**
 *
 * @param fd	File descriptor
 * @param buf	buffer
 * @param nSz	Size of buffer
 * @param io	I/O mode (Read/Write)
 * @return
 */
static int httpd_svc_io(int fd, char *buf, size_t nSz, int io)
{
	fd_set fds;
	struct timeval timeout;
	int ret, returnSz = 0;

	while (1) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		// set timeout
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		if (io == HTTPD_IO_RD) {
			ret = dgt_os_select(fd + 1, &fds, NULL, NULL, &timeout);
		} else {
			ret = dgt_os_select(fd + 1, NULL, &fds, NULL, &timeout);
		}

		if (ret == -1) {
			dgt_os_perror("<<httpd_svc_get_http_req>>");
			returnSz = -1;
			break;
		} else if (ret) {
			// data available
			if (io == HTTPD_IO_RD) {
				returnSz = dgt_os_recv(fd, buf, nSz, 0);
			} else {
				returnSz = dgt_os_send(fd, buf, nSz, 0);
			}
			break;
		} else {
			printf("%s : Data timeout \n", __FUNCTION__);
			break;
		}
	}

	return returnSz;
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

static int httpd_svc_resp(int client_fd, struct httpd_conf *pConfig)
{
	char work_buff[HTTPD_RESP_MAX];
	int nIoSz;

	if (!(pConfig)) {
		return -1;
	}

	memset(work_buff, 0x0, HTTPD_RESP_MAX);

	// read response
	nIoSz = httpd_svc_io(pConfig->loc.fd, work_buff, HTTPD_RESP_MAX, HTTPD_IO_RD);
	HTTPDSVC_LOG_TRACE("wr-Sz.%d err.%d\n", nIoSz, errno);

	// process respons
	nIoSz = httpd_svc_io(client_fd, work_buff, strlen (work_buff), HTTPD_IO_WR);

	dgt_os_close(pConfig->loc.fd);
}

int httpd_svc_req_proc(int client_fd, struct httpd_conf * config)
{
	char work_buff[HTTPD_RESP_MAX];
	int nIoSz;
	int ret;

	// read HTTP request
	nIoSz = httpd_svc_io(client_fd, work_buff, HTTPD_RESP_MAX, HTTPD_IO_RD);
	HTTPDSVC_LOG_TRACE("http-req-sz.%d err.%d\n", nIoSz, errno);

	if (nIoSz > 0) {

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
									HTTPD_IO_WR);
			HTTPDSVC_LOG_TRACE("notify-ntp-sz.%d err.%d\n", nIoSz, errno);

			httpd_svc_resp(client_fd, config);
		} else {
			HTTPDSVC_LOG_TRACE("<<connect>> FAIL, err.%d", errno);
		}
	}
}