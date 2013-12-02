/*
 * File:   dgt_ntp_op.h
 * Author: dyn
 *
 * Created on November 16, 2013, 5:24 PM
 */

#ifndef DGT_NTP_OP_H
#define	DGT_NTP_OP_H

#ifdef	__cplusplus
extern "C" {
#endif

int dgt_ntp_get_http_req(int fd, char *buf, size_t nSz);
int dgt_ntp_http_req_io(int fd, char *buf, size_t nSz, int io);
int dgt_ntp_http_req_select(int fd);

#ifdef	__cplusplus
}
#endif

#endif	/* DGT_NTP_OP_H */

