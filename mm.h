// MM - a simplistic memory manager / garbage collector skeleton.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

// provides a malloc-like interface to allocate chunks of memory.
// To free the memory (and call optional finalizing functions)
// do a mm_start(), then mark the objects that should survive the purge
// with a mm_protect(), then execute mm_gc().
// It's your task to mm_protect() the future survivors!

#ifndef __MM_H
#define __MM_H

#include "common.h"

typedef struct {
	bool protect;
	void* data;
	void (*on_finalize)(void* data);
	void (*on_protect)(void* data);
} MMEntry;

typedef struct {
	MMEntry* entries;
	unsigned int size;
	unsigned int capacity;
} MM;

MM*   mm_create(void);
void* mm_alloc(MM* mm, size_t size, void (*optional_on_finalize)(void* data),
			void (*optional_on_protect)(void* data));
void  mm_manage(MM* mm, void* data, void (*optional_on_finalize)(void* data),
			void (*optional_on_protect)(void* data));
void* mm_realloc(MM* mm, void* data, size_t size);
void  mm_start_gc(MM* mm);
void  mm_protect(MM* mm, void* data);
void  mm_gc(MM* mm);
bool  mm_is_managing(MM* mm, void* data);
bool  mm_is_protected(MM* mm, void* data);
void  mm_destroy(MM* mm);

#endif
