/**
 * \file dgt_sys_mem.c
 * \desc Memory API
 *
 * Copyright(c) 2013 Neil Dy
 */

#include <dgt_config.h>

static int dbg_alloc_cntr = 0;

void *dgt_sys_alloc(size_t size)
{
	void *p;

	dbg_alloc_cntr++;

	p = malloc(size);
	if (p == NULL) {
		dbg_alloc_cntr--;
	}

	return p;
}

void *dgt_sys_calloc(size_t size)
{
	void *p;


	p = dgt_sys_alloc(size);
	if (p != NULL) {
		memset(p, 0x0, size);
	}

	return p;
}

void dgt_sys_free(void *p)
{
	free(p);
	dbg_alloc_cntr--;
}