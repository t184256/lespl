// MM - a simplistic memory manager / garbage collector skeleton.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#include "mm.h" // Please refer to it for a general overview of mm

/*
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
*/

#define MM_MIN_CAPACITY 0 /*16*/
#define MM_CAPACITY_INCREMENT(old) (old + 1/*8*/)
#define MM_NOT_FOUND UINT_MAX

static void mm_finalize(MMEntry mme);
static void mm_resize(MM* mm, uint capacity);
static void mm_protect_mmentry(MMEntry* mme);
static uint mm_find(MM* mm, void* data);

MM* mm_create(void) {
	MM* mm = (MM*) malloc(sizeof(MM));
	if (!mm) MEM_DIE;
	mm->size = 0;
	mm->capacity = MM_MIN_CAPACITY;
	mm->entries = (MMEntry*) malloc(sizeof(MMEntry) * mm->capacity);
	//dbg_out("MM created %p\n", mm);
	return mm;
}

void* mm_alloc(MM* mm, size_t size, void (*optional_on_finalize)(void* data),
		void (*optional_on_protect)(void* data)) {
	void* mem = malloc(size);
	if (!mem) MEM_DIE; // TODO: better error handling
	mm_manage(mm, mem, optional_on_finalize, optional_on_protect);
	return mem;
}

void mm_manage(MM* mm, void* data, void (*optional_on_finalize)(void* data),
		void (*optional_on_protect)(void* data)) {
	uint i = mm_find(mm, data);
	if (i != MM_NOT_FOUND) {
		// Allow omitting or setting, but not changing or clearing
		MMEntry mme = mm->entries[i];
		if (optional_on_finalize) {
			assert(mme.on_finalize == optional_on_finalize);
			mm->entries[i].on_finalize = optional_on_finalize;
		}
		if (optional_on_protect) {
			assert(mme.on_protect == optional_on_protect);
			mm->entries[i].on_protect = optional_on_protect;
		}
		return;
	}
	if (mm->size == mm->capacity) {
		mm_resize(mm, MM_CAPACITY_INCREMENT(mm->capacity));
		assert(mm->capacity < UINT_MAX);
		assert(MM_CAPACITY_INCREMENT(mm->capacity) < UINT_MAX);
	}
	mm->entries[mm->size].data = data;
	mm->entries[mm->size].protect = 0;
	mm->entries[mm->size].on_finalize = optional_on_finalize;
	mm->entries[mm->size].on_protect = optional_on_protect;
	mm->size++;
}

void* mm_realloc(MM* mm, void* data, size_t size) {
	uint i = mm_find(mm, data);
	assert(i != MM_NOT_FOUND);
	mm->entries[i].data = realloc(mm->entries[i].data, size);
	if (!mm->entries[i].data) MEM_DIE;
	return mm->entries[i].data;
}

static void mm_resize(MM* mm, uint capacity) {
	dbg_assert(capacity >= mm->size);
	mm->capacity = capacity > MM_MIN_CAPACITY ? capacity : MM_MIN_CAPACITY;
	mm->entries = realloc(mm->entries, sizeof(MMEntry) * mm->capacity);
	if (!mm->entries) MEM_DIE;
}

void mm_start_gc(MM* mm) {
	uint i;
	for (i = 0; i < mm->size; i++)
		mm->entries[i].protect = false;
}

void mm_protect(MM* mm, void* data) {
	uint i;
	for (i = 0; i < mm->size; i++)
		if (mm->entries[i].data == data)
			mm_protect_mmentry(&mm->entries[i]);
}

static void mm_protect_mmentry(MMEntry* mme) {
	if (mme->on_protect)
		mme->on_protect(mme->data);
	mme->protect = true;
}

void mm_gc(MM* mm) {
	uint i, p = 0;
//	for (i = 0; i < mm->size; i++)
//		printf("was %u %s\n", i, mm->entries[i].data);
	for (i = 0; i < mm->size; i++) {
//		printf("Protect %d; i=%u p=%u\n", mm->entries[i].protect, i, p);
		if (mm->entries[i].protect)
			mm->entries[p++] = mm->entries[i];
		else
			mm_finalize(mm->entries[i]);
	}
	mm->size = p;
//	for (i = 0; i < mm->size; i++)
//		printf("now %u %s\n", i, mm->entries[i].data);
//	printf("size %u capacity %u\n", mm->size, mm->capacity);
	mm_resize(mm, mm->size);
}

static void mm_finalize(MMEntry mme) {
	if (mme.on_finalize)
		mme.on_finalize(mme.data);
	/*
	dbg_out("free %p:%c%c%c%c\n", mme.data,
			((char*) mme.data)[0],
			((char*) mme.data)[1],
			((char*) mme.data)[2],
			((char*) mme.data)[3]);
	*/
	if (mme.data) free(mme.data);
}

static uint mm_find(MM* mm, void* data) {
	uint i;
	for (i = 0; i < mm->size; i++)
		if (mm->entries[i].data == data)
			return i;
	return MM_NOT_FOUND;
}

bool mm_is_managing(MM* mm, void* data) {
	return mm_find(mm, data) != MM_NOT_FOUND;
}

bool mm_is_protected(MM* mm, void* data) {
	uint i;
	for (i = 0; i < mm->size; i++)
		if (mm->entries[i].data == data)
			return mm->entries[i].protect;
	return false;
}

void mm_destroy(MM* mm) {
	uint i;
	for (i = 0; i < mm->size; i++)
		mm_finalize(mm->entries[i]);
	free(mm->entries);
	free(mm);
}
