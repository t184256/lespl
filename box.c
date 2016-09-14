// Box - an ordered map container.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#include "box.h" // Please refer to it for a general overview of box

/*
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
*/

#define BOX_MIN_CAPACITY 0 /*8*/
#define BOX_CAPACITY_INCREMENT(old) (old + 1/*4*/)
#define BOX_NOT_FOUND UINT_MAX

static uint box_index(Box* box, const char* key);
static void box_resize(Box* box, uint capacity);
static void box_protect_entries(void* box);

Box* box_create(MM* optional_mm) {
	MM* mm = optional_mm ? optional_mm : mm_create();
	Box* box = mm_alloc(mm, sizeof(Box), NULL, box_protect_entries);
	box->mm = mm;
	box->size = 0;
	box->capacity = BOX_MIN_CAPACITY;
	box->entries = mm_alloc(mm, sizeof(BoxEntry) * box->capacity, NULL, NULL);
	return box;
}

static void box_protect_entries(void* box) {
	Box* b = box;
	uint i;
	if (mm_is_protected(b->mm, b->entries))
		return; // Exit the recursion breaking dependency loops.
	mm_protect(b->mm, b->entries);
	for (i = 0; i < b->size; i++) {
		mm_protect(b->mm, b->entries[i].key);
		mm_protect(b->mm, b->entries[i].value);
	}
}

void box_add(Box* box, const char* key, void* value) {
	uint i = box_index(box, key);
	assert(box->capacity < UINT_MAX);
	assert(BOX_CAPACITY_INCREMENT(box->capacity) < UINT_MAX);
	if (box->size == box->capacity)
		box_resize(box, BOX_CAPACITY_INCREMENT(box->capacity));
	i = box->size++;
	box->entries[i].key = strdup(key);
	mm_manage(box->mm, box->entries[i].key, NULL, NULL);
	box->entries[i].value = value;
	mm_manage(box->mm, value, NULL, NULL);
}

void box_set(Box* box, const char* key, void* value) {
	//dbg_out("set %s %p\n", key, value);
	uint i = box_index(box, key);
	if (i == BOX_NOT_FOUND)
		return box_add(box, key, value);
	box->entries[i].value = value;
	mm_manage(box->mm, value, NULL, NULL);
}

static void box_resize(Box* box, uint capacity) {
	dbg_assert(capacity >= box->size);
	capacity = capacity > BOX_MIN_CAPACITY ? capacity : BOX_MIN_CAPACITY;
	box->capacity = capacity;
	box->entries = mm_realloc(box->mm, box->entries, sizeof(BoxEntry) * box->capacity);
	if (!box->entries) MEM_DIE;
}

static uint box_index(Box* box, const char* key) {
	uint i;
	for (i = 0; i < box->size; i++)
		if (!strcmp(box->entries[i].key, key))
			return i;
	return BOX_NOT_FOUND;
}

void* box_get(Box* box, const char* key) {
	uint i = box_index(box, key);
	if (i == BOX_NOT_FOUND) {
		return NULL;
	}
	return box->entries[i].value;
}

bool box_has(Box* box, const char* key) {
	return box_index(box, key) != BOX_NOT_FOUND;
}

void box_del(Box* box, const char* key) {
	uint i = box_index(box, key);
	assert(i != BOX_NOT_FOUND);
	box_nth_del(box, i);
}

void* box_nth(Box* box, unsigned int i) {
	return box->entries[i].value;
}

char* box_nth_key(Box* box, unsigned int i) {
	return box->entries[i].key;
}

void box_nth_del(Box* box, unsigned int i) {
	uint j;
	for (j = i + 1; j < box->size; j++)
		box->entries[j - 1] = box->entries[j];
	box->size--;
	//dbg_out("{\n");
	//for (j = 0; j < box->size; j++)
	//	dbg_out("%s:%p\n", box->entries[j].key, box->entries[j].value);
	//dbg_out("}\n");
}

uint box_size(Box* box) {
	return box->size;
}

Box* box_clone(Box* box) {
	MM* mm = box->mm;
	Box* n = mm_alloc(mm, sizeof(Box), NULL, box_protect_entries);
	n->mm = mm;
	n->size = box->size;
	n->capacity = n->size > BOX_MIN_CAPACITY ? n->size : BOX_MIN_CAPACITY;
	n->entries = mm_alloc(mm, sizeof(BoxEntry) * n->capacity, NULL, NULL);
	memcpy(n->entries, box->entries, sizeof(BoxEntry) * n->size);
	return n;
}

void box_protect_recursively(Box* box) {
	mm_protect(box->mm, box); // NOTE: this recurses with callbacks!
}

MM* box_mm(Box* box) {
	return box->mm;
}

void box_cleanup_except(Box* box) {
	mm_start_gc(box->mm);
	mm_protect(box->mm, box);
	mm_gc(box->mm);
}

void box_cleanup_all(Box* box) {
	mm_destroy(box->mm);
}

//void box_destroy(Box* box); // could it be omitted for simplicity?

