#ifndef _COMMON_H
#define _COMMON_H

#include <assert.h>
#include <ctype.h>
#include <dlfcn.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#define dbg_assert(condition) assert(condition)
#define dbg_out(...) fprintf(stderr, __VA_ARGS__)
#define DBG fprintf(stderr, "%s:%s:%d.\n", __FILE__, __FUNCTION__, __LINE__)
#else
#define dbg_assert(condition)
#define DBG
#define dbg_out(...)
#endif

#define die(...) do { \
	fprintf(stderr, "! "); \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "! \n"); \
	exit(EXIT_FAILURE); } while (0)
#define MEM_DIE die("Memory error");
// TODO: better error handling

typedef unsigned int uint;

#endif
