// libbool - a library providing booleans in lespl.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#include "libbool.h"
#include "libskip.h"

static bool* lespl_libbool_box(MM* mm, bool val);

static Box* lespl_libbool_new_(Box* clb, Box* arg, Box* ctx);
static bool* lespl_libbool_box(MM* mm, bool val) {
	bool* lbb = mm_alloc(mm, sizeof(bool*), NULL, NULL);
	*lbb = val;
	return lbb;
}

Box* lespl_libbool_inverted(Box* lespl_bool) {
	Box* inv = box_clone(lespl_bool);
	box_set(inv, "__bool_value__", lespl_libbool_box(box_mm(lespl_bool),
				!lespl_libbool_unbox(lespl_bool)));
	return inv;
}

Box* lespl_libbool_main(Box* clb, Box* unused_arg, Box* ctx) {
	MM* mm = box_mm(ctx);

	Box* b = box_create(mm);
	box_set(b, "__bool_value__", lespl_libbool_box(mm, 0));

	lespl_add_method(b, "true",	lespl_libbool_true);
	lespl_add_method(b, "false",	lespl_libbool_false);

	lespl_add_method(b, "new",	lespl_libbool_new);
	lespl_add_method(b, "not",	lespl_libbool_not);
	lespl_add_method(b, "and",	lespl_libbool_and);
	lespl_add_method(b, "or",	lespl_libbool_or);
	lespl_add_method(b, "xor",	lespl_libbool_xor);

	return b;
}

Box* lespl_libbool_true(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__bool_value__"));
	Box* x = box_clone(arg);
	box_set(x, "__bool_value__", lespl_libbool_box(box_mm(ctx), 1));
	return x;
}

Box* lespl_libbool_false(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__bool_value__"));
	Box* x = box_clone(arg);
	box_set(x, "__bool_value__", lespl_libbool_box(box_mm(ctx), 0));
	return x;
}

Box* lespl_libbool_and(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__bool_value__"));
	// Short-circuit: next arg on true, false on false
	return lespl_libbool_unbox(arg) ? NULL : lespl_libskip_skippy(arg);
}

Box* lespl_libbool_or(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__bool_value__"));
	// Short-circuit: self on true (skipping next arg), next arg on false
	return lespl_libbool_unbox(arg) ? lespl_libskip_skippy(arg) : NULL;
}

Box* lespl_libbool_xor(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__bool_value__"));
	// Short-circuit: false on true, next arg on false
	return lespl_libbool_unbox(arg) ?
		lespl_libskip_skippy(lespl_libbool_inverted(arg)) : NULL;
}

Box* lespl_libbool_create(MM* mm, bool b) {
	Box* z = lespl_libbool_main(box_create(mm),
			box_create(mm), box_create(mm));
	box_set(z, "__bool_value__", lespl_libbool_box(mm, b));
	return z;
}

bool lespl_libbool_unbox(Box* lespl_bool) {
	assert(box_has(lespl_bool, "__bool_value__"));
	return *((bool*) box_get(lespl_bool, "__bool_value__"));
}

Box* lespl_libbool_new(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__bool_value__"));
	Box* c = lespl_create_callable(box_mm(ctx), lespl_libbool_new_);
	box_set(c, "__prev_bool__", arg);
	return c;
}

static Box* lespl_libbool_new_(Box* clb, Box* arg, Box* ctx) {
	// TODO: accept bools?
	Box* s = box_clone(box_get(clb, "__prev_bool__"));
	bool b = !strcmp("true", lespl_peel(arg));
	box_set(s, "__bool_value__", lespl_libbool_box(box_mm(ctx), b));
	return s;
}

Box* lespl_libbool_not(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__bool_value__"));
	bool* val = box_get(arg, "__bool_value__");
	bool b = !*val;
	Box* s = box_clone(arg);
	box_set(s, "__bool_value__", lespl_libbool_box(box_mm(ctx), b));
	return s;
}

