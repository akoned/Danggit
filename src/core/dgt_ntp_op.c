/**
 * \file dgt_ntp_op.c
 *
 * Copyright(c) 2013 Neil Dy
 */
#include <stdlib.h>

#include "dgt_config.h"
#include "dgt_os_io.h"

#include "dgt_ntp.h"

int dgt_ntp_get_http_req(int fd, char *buf, size_t nSz)
{
	fd_set rdfds;
	struct timeval timeout;
	int ret, nRdSz = 0;

	while (1) {
		FD_ZERO(&rdfds);
		FD_SET(fd, &rdfds);

		// set timeout
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;

		ret = dgt_os_select(fd + 1, &rdfds, NULL, NULL, &timeout);

		if (ret == -1) {
			dgt_os_perror("<<dgt_ntp_get_http_req>>");
			nRdSz = -1;
			break;
		} else if (ret) {
			// data available
			nRdSz = dgt_os_read(fd, buf, nSz);
			break;
		} else {
			printf("%s : data not yet ready\n", __FUNCTION__);
		}
	}

	return nRdSz;
}

int dgt_ntp_http_req_io(int fd, char *buf, size_t nSz, int io)
{
	fd_set fds;
	struct timeval timeout;
	int ret, nReturnSz = 0;

	while (1) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		// set timeout
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;

		if (DGT_NTP_IO_RD == io) {
			ret = dgt_os_select(fd + 1, &fds, NULL, NULL, &timeout);
		} else {
			ret = dgt_os_select(fd + 1, NULL, &fds, NULL, &timeout);
		}


		if (ret == -1) {
			dgt_os_perror("<<dgt_ntp_get_http_req>>");
			nReturnSz = -1;
			break;
		} else if (ret) {
			// data available
			if (DGT_NTP_IO_RD == io) {
				nReturnSz = dgt_os_read(fd, buf, nSz);
			} else {
				nReturnSz = dgt_os_write(fd, buf, nSz);
			}
			break;
		} else {
			printf("%s : data not yet ready\n", __FUNCTION__);
		}
	}

	return nReturnSz;
}

int dgt_ntp_http_req_select(int fd)
{
	fd_set fds;
	struct timeval timeout;
	int ret;
	int new_conn = -1;

	while (1) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		// set timeout
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		ret = dgt_os_select(fd + 1, &fds, NULL, NULL, &timeout);

		if (ret < 0) {
			dgt_os_perror("dgt_ntp_http_req_select FAIL!!!");
			break;
		} else if (ret == 1) {
			if (FD_ISSET(fd, &fds)) {
				new_conn = dgt_os_accept(fd, NULL, NULL);
				break;
			} else {
				//
			}
		} else {
			// timeout expire
		}
	}

	return new_conn;
}
