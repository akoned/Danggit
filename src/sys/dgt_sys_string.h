/*
 * File:   dgt_sys_string.h
 * Author: dyn
 *
 * Created on November 25, 2013, 5:11 PM
 */

#ifndef DGT_SYS_STRING_H
#define	DGT_SYS_STRING_H

#ifdef	__cplusplus
extern "C" {
#endif

size_t dgt_sys_strcpy(const char *src, size_t srcSz, char *dest, size_t destSz);
size_t dgt_sys_strlen(const char *src);
char * dgt_sys_strstr(const char *src, const char *search);
char * dgt_sys_strncat(char * destination, const char * source, size_t num);
int dgt_sys_strncmp(const char * s1, const char * s2, size_t num);

#ifdef	__cplusplus
}
#endif

#endif	/* DGT_SYS_STRING_H */

