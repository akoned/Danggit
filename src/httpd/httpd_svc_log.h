/*
 * File:   httpd_svc_log.h
 * Author: dyn
 *
 * Created on November 22, 2013, 3:31 PM
 */

#ifndef HTTPD_SVC_LOG_H
#define	HTTPD_SVC_LOG_H

#ifdef	__cplusplus
extern "C" {
#endif

#define HTTPDSVC_LOG_TRACE(FMT, ...)\
	fprintf(stderr, "[%s %d] "FMT"\n", __FUNCTION__, __LINE__, ## __VA_ARGS__)

#define HTTPDSVC_LOG_SYS(FMT, ...)\
	fprintf(stderr, "[HTTPD System Log (%s, %d)] "FMT"\n", __FUNCTION__, __LINE__, ## __VA_ARGS__)


#ifdef	__cplusplus
}
#endif

#endif	/* HTTPD_SVC_LOG_H */

