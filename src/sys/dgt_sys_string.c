/**
 * \file dgt_sys_mem.c
 * \desc String API
 *
 * Copyright(c) 2013 Neil Dy
 */
#include "dgt_config.h"
#include "dgt_sys_string.h"

size_t dgt_sys_strcpy(const char *src, size_t srcSz, char *dest, size_t destSz)
{
	size_t nCpySz;

	if (src == NULL || dest == NULL) return 0;

	nCpySz = srcSz;
	if (nCpySz > destSz) {
		nCpySz = destSz;
	}

	memcpy(dest, src, nCpySz);

	return nCpySz;
}

size_t dgt_sys_strlen(const char *src)
{
	if (src != NULL)
		return strlen(src);
	else
		return 0;
}

char * dgt_sys_strstr(const char *source, const char *search)
{
	if (source == NULL || search == NULL) return NULL;

	return strstr(source, search);
}

char * dgt_sys_strncat(char * destination, const char * source, size_t num)
{
	if (destination == NULL || source == NULL) return NULL;

	return strncat(destination, source, num);
}

int dgt_sys_strncmp(const char * s1, const char * s2, size_t num)
{
	if (s1 == NULL || s2 == NULL) return -1;

	return strncmp(s1, s2, num);
}