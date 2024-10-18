#include "jary/memory.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *sb_alloc(struct sb_mem *sb, uint32_t nmemb)
{
	const uint32_t growth = 2;
	const uint32_t nextsz = sb->size + nmemb;

	if (nextsz >= sb->capacity) {
		uint32_t newsz = (sb->capacity + nmemb) * growth;
		void	*mem   = realloc(sb->buf, newsz);

		if (mem == NULL)
			goto OUT_OF_MEMORY;

		sb->capacity = newsz;
		sb->buf	     = mem;
	}

	sb->size = nextsz;

	return sb->buf;
OUT_OF_MEMORY:
	return NULL;
}

void *sb_reserve(struct sb_mem *sb, uint32_t nmemb)
{
	const uint32_t growth = 2;
	const uint32_t nextsz = sb->size + nmemb;

	if (nextsz >= sb->capacity) {
		uint32_t newsz = (sb->capacity + nmemb) * growth;
		void	*mem   = realloc(sb->buf, newsz);

		if (mem == NULL)
			goto OUT_OF_MEMORY;

		sb->capacity = newsz;
		sb->buf	     = mem;
	}

	return sb->buf;
OUT_OF_MEMORY:
	return NULL;
}

void sb_free(struct sb_mem *sb)
{
	free(sb->buf);
}

void *sc_alloc(struct sc_mem *alloc, uint32_t nmemb)
{
	void	      *block = calloc(nmemb, 1);
	struct sc_mem *back  = NULL;

	if (block == NULL)
		goto OUT_OF_MEMORY;

	back = malloc(sizeof *alloc);

	if (back == NULL)
		goto OUT_OF_MEMORY;

	back->buf    = block;
	back->expire = free;
	back->back   = alloc->back;
	alloc->back  = back;

	return block;

OUT_OF_MEMORY:
	free(block);
	free(alloc->back);
	return NULL;
}

void *su_alloc(struct su_mem *alloc, void *scptr, uint32_t nmemb)
{
	struct su_w *buf = NULL;

	if (scptr == NULL) {
		buf = calloc(sizeof *buf + nmemb, 1);

		if (buf == NULL)
			goto FAIL;

		alloc->size += 1;
		buf->self    = alloc;
		buf->ord     = alloc->size - 1;
		buf->size    = nmemb;

		alloc->buf = realloc(alloc->buf,
				     sizeof(struct su_w *) * alloc->size);

		alloc->buf[alloc->size - 1] = buf;
	} else {
		struct su_w *temp = (struct su_w *) scptr - 1;

		assert(temp->self == alloc);

		buf = calloc(sizeof *buf + nmemb + temp->size, 1);

		if (buf == NULL)
			goto FAIL;

		memcpy(buf, temp, sizeof(*buf) + temp->size);
		buf->size	      += nmemb;
		alloc->buf[temp->ord]  = buf;

		free(temp);
	}

	return buf->ptr;
FAIL:
	return NULL;
}

void *sc_allocf(struct sc_mem *alloc, uint32_t nmemb, free_t expire)
{
	void *block = calloc(nmemb, 1);

	if (block == NULL)
		return NULL;

	struct sc_mem *temp = alloc->back;
	alloc->back	    = calloc(sizeof *alloc, 1);

	if (alloc->back == NULL)
		return NULL;

	alloc->back->buf    = block;
	alloc->back->expire = expire;
	alloc->back->back   = temp;

	return block;
}

int sc_strfmt(struct sc_mem *alloc, char **str, const char *fmt, ...)
{
	va_list arg;

	va_start(arg, fmt);

	int sz = vasprintf(str, fmt, arg);

	if (*str == NULL)
		goto OUT_OF_MEMORY;

	struct sc_mem *oldback = alloc->back;
	struct sc_mem *back    = (struct sc_mem *) calloc(sizeof *alloc, 1);

	if (back == NULL)
		goto OUT_OF_MEMORY;

	alloc->back	    = back;
	alloc->back->buf    = *str;
	alloc->back->expire = free;
	alloc->back->back   = oldback;

	return sz;

OUT_OF_MEMORY:
	free(*str);
	return -1;
}

int sc_reap(struct sc_mem *alloc, void *buf, free_t expire)
{
	struct sc_mem *temp = alloc->back;
	alloc->back	    = calloc(sizeof *alloc, 1);

	if (alloc->back == NULL)
		goto OUT_OF_MEMORY;

	alloc->back->buf    = buf;
	alloc->back->expire = expire;
	alloc->back->back   = temp;

	return 0;

OUT_OF_MEMORY:
	return -1;
}

void sc_free(struct sc_mem *alloc)
{
	void	      *buf     = alloc->buf;
	struct sc_mem *next    = alloc->back;
	void (*expire)(void *) = alloc->expire;

	for (;;) {
		if (expire)
			expire(buf);

		if (next == NULL)
			break;

		void *temp = next;

		buf    = next->buf;
		expire = next->expire;
		next   = next->back;

		free(temp);
	}
}

void su_free(struct su_mem *alloc)
{
	for (int i = 0; i < alloc->size; ++i) {
		struct su_w *buf = alloc->buf[i];

		free(buf);
	}

	free(alloc->buf);
}