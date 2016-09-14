// libint - a library providing integers in lespl.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#include "libint.h"
#include "libbool.h"

static int64_t* lespl_libint_box(MM* mm, int64_t val);
static int64_t lespl_libint_atoll(const char* s);

static Box* lespl_libint_new_(Box* clb, Box* arg, Box* ctx);
static Box* lespl_libint_add_(Box* clb, Box* arg, Box* ctx);
static Box* lespl_libint_subtract_(Box* clb, Box* arg, Box* ctx);
static Box* lespl_libint_equals_(Box* clb, Box* arg, Box* ctx);

static int64_t* lespl_libint_box(MM* mm, int64_t val) {
	int64_t* lbi = mm_alloc(mm, sizeof(int64_t*), NULL, NULL);
	*lbi = val;
	return lbi;
}

static int64_t lespl_libint_atoll(const char* s) {
	char* endptr;
	if (!(*s)) { fprintf(stderr, "! empty int to parse\n"); exit(1); }
	int64_t i = strtoll(s, &endptr, 0);
	if (*endptr) { fprintf(stderr, "! bad int %s\n", s); exit(1); }
	return i;
	// TODO: test for overflow/underflow?
}

Box* lespl_libint_main(Box* clb, Box* unused_arg, Box* ctx) {
	MM* mm = box_mm(ctx);

	Box* s = box_create(mm);
	box_set(s, "__int_value__", lespl_libint_box(mm, 0));

	lespl_add_method(s, "new",	lespl_libint_new);
	lespl_add_method(s, "negate",	lespl_libint_negate);
	lespl_add_method(s, "add",	lespl_libint_add);
	lespl_add_method(s, "subtract",	lespl_libint_subtract);
	lespl_add_method(s, "equals",	lespl_libint_equals);

	return s;
}

Box* lespl_libint_add(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__int_value__"));
	Box* c = lespl_create_callable(box_mm(ctx), lespl_libint_add_);
	box_set(c, "__prev_int__", arg);
	return c;
}

static Box* lespl_libint_add_(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(clb, "__prev_int__"));
	Box* prv = box_get(clb, "__prev_int__");
	assert(box_has(prv, "__int_value__"));
	assert(box_has(arg, "__int_value__"));
	int64_t* val_a = box_get(prv, "__int_value__");
	int64_t* val_b = box_get(arg, "__int_value__");
	int64_t i = *val_a + *val_b;
	Box* s = box_clone(prv);
	box_set(s, "__int_value__", lespl_libint_box(box_mm(ctx), i));
	return s;
}

Box* lespl_libint_subtract(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__int_value__"));
	Box* c = lespl_create_callable(box_mm(ctx), lespl_libint_subtract_);
	box_set(c, "__prev_int__", arg);
	return c;
}

static Box* lespl_libint_subtract_(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(clb, "__prev_int__"));
	Box* prv = box_get(clb, "__prev_int__");
	assert(box_has(prv, "__int_value__"));
	assert(box_has(arg, "__int_value__"));
	int64_t* val_a = box_get(prv, "__int_value__");
	int64_t* val_b = box_get(arg, "__int_value__");
	int64_t i = *val_a - *val_b;
	Box* s = box_clone(prv);
	box_set(s, "__int_value__", lespl_libint_box(box_mm(ctx), i));
	return s;
}

Box* lespl_libint_equals(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__int_value__"));
	Box* c = lespl_create_callable(box_mm(ctx), lespl_libint_equals_);
	box_set(c, "__prev_int__", arg);
	return c;
}

Box* lespl_libint_equals_(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(clb, "__prev_int__"));
	Box* prv = box_get(clb, "__prev_int__");
	assert(box_has(prv, "__int_value__"));
	assert(box_has(arg, "__int_value__"));
	int64_t* a = box_get(prv, "__int_value__");
	int64_t* b = box_get(arg, "__int_value__");
	return lespl_libbool_create(box_mm(ctx), *a == *b);
}

Box* lespl_libint_create_from_int(MM* mm, int64_t i) {
	Box* z = lespl_libint_main(NULL, NULL, box_create(mm));
	box_set(z, "__int_value__", lespl_libint_box(mm, i));
	return z;
}

Box* lespl_libint_create_from_string(MM* mm, const char* string) {
	return lespl_libint_create_from_int(mm, lespl_libint_atoll(string));
}

int64_t lespl_libint_unbox(Box* lespl_int) {
	assert(box_has(lespl_int, "__int_value__"));
	return *((int64_t*) box_get(lespl_int, "__int_value__"));
}

Box* lespl_libint_new(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__int_value__"));
	Box* c = lespl_create_callable(box_mm(ctx), lespl_libint_new_);
	box_set(c, "__prev_int__", arg);
	return c;
}

static Box* lespl_libint_new_(Box* clb, Box* arg, Box* ctx) {
	// TODO: accept ints?
	int64_t i = lespl_libint_atoll(lespl_peel(arg));
	Box* s = box_clone(box_get(clb, "__prev_int__"));
	box_set(s, "__int_value__", lespl_libint_box(box_mm(ctx), i));
	return s;
}

Box* lespl_libint_negate(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__int_value__"));
	int64_t* val = box_get(arg, "__int_value__");
	int64_t i = - *val;
	Box* s = box_clone(arg);
	box_set(s, "__int_value__", lespl_libint_box(box_mm(ctx), i));
	return s;
}
