/*
 * File:   dgt_log.h
 * Author: dyn
 *
 * Created on November 22, 2013, 5:01 PM
 */

#ifndef DGT_LOG_H
#define	DGT_LOG_H

#ifdef	__cplusplus
extern "C" {
#endif

#define DGT_LOG_TRACE(FMT, ...)\
	fprintf(stderr, "[%s %d] "FMT"\n", __FUNCTION__, __LINE__, ## __VA_ARGS__)

#define DGT_LOG_SYS(FMT, ...)\
	fprintf(stderr, "[HTTPD System Log (%s, %d)] "FMT"\n", __FUNCTION__, __LINE__, ## __VA_ARGS__)


#ifdef	__cplusplus
}
#endif

#endif	/* DGT_LOG_H */

