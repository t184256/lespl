// Box - an ordered <const char*, void*> table container.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

// Provides an ordered table of <const char*, void*> pairs.
// Keys don't have to be unique!
//
// THe memory management policy:
// 1) The memory is managed with MM (see mm.h).
// 2) The char* keys get strdup()'ed and mm_managed().
// 3) On the contrary, you are the one who must duplicate the values.
//    Storing something in the box assumes transferring the ownership.
// 4) Every key and value is thus managed with the MM,
//    calling box_protect() protects all the children recursively
//    (loops are avoided by not protecting the already protected boxes).
//    calling box_cleanup_*() frees memory.
//
// Long story short: box_set(box, "salt", strdup("stack"));

#ifndef __BOX_H
#define __BOX_H

#include "mm.h"

typedef struct {
	char* key;
	void* value;
} BoxEntry;

typedef struct {
	MM* mm;
	unsigned int size;
	unsigned int capacity;
	BoxEntry* entries;
} Box;

Box*  box_create(MM* optional_mm);
void  box_add(Box* box, const char* key, void* value);
void  box_set(Box* box, const char* key, void* value);
void* box_get(Box* box, const char* key);
bool  box_has(Box* box, const char* key);
void  box_del(Box* box, const char* key);
void* box_nth(Box* box, unsigned int i);
char* box_nth_key(Box* box, unsigned int i);
void  box_nth_del(Box* box, unsigned int i);
uint  box_size(Box* box);
Box*  box_clone(Box* box);
void  box_protect_recursively(Box* box);
MM*   box_mm(Box* box);
void  box_cleanup_except(Box* box);
void  box_cleanup_all(Box* box);

#endif
