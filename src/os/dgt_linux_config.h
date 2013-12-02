/*
 * File:   dgt_linux_config.h
 * Author: dyn
 *
 * Created on November 5, 2013, 11:45 AM
 */

#ifndef DGT_LINUX_CONFIG_H
#define	DGT_LINUX_CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <pthread.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>

#ifdef	__cplusplus
}
#endif

#endif	/* DGT_LINUX_CONFIG_H */
