/**
 * \file pltw_os_io.c
 *
 * Copyright(c) 2013 Neil Dy
 */
#include "pltw_config.h"

int hok_send(int fd, void *buf, size_t size)
{
	return 0;
}

int hok_rcv(int fd, void *buf, size_t size)
{
	return 0;
}

int hok_select(int fd, fd_set *readfds, fd_set *writefds,
				fd_set *exceptfds, struct timeval *timeout)
{
	return 0;
}