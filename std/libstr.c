// libstr - a library providing strings in lespl.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#include "../liblespl.h"
#include "libint.h"

Box* lespl_libstr_main(Box* clb, Box* arg, Box* ctx);
Box* lespl_libstr_new(Box* clb, Box* arg, Box* ctx);
Box* lespl_libstr_new_(Box* clb, Box* arg, Box* ctx);
Box* lespl_libstr_reverse(Box* clb, Box* arg, Box* ctx);
Box* lespl_libstr_concat(Box* clb, Box* arg, Box* ctx);
Box* lespl_libstr_concat_(Box* clb, Box* arg, Box* ctx);
Box* lespl_libstr_length(Box* clb, Box* arg, Box* ctx);
Box* lespl_libstr_get(Box* clb, Box* arg, Box* ctx);
Box* lespl_libstr_get_(Box* clb, Box* arg, Box* ctx);


Box* lespl_libstr_main(Box* clb, Box* unused_arg, Box* ctx) {
	MM* mm = box_mm(ctx);

	Box* s = box_create(mm);
	box_set(s, "__str_value__", strdup(""));

	lespl_add_method(s, "new", lespl_libstr_new);

	lespl_add_method(s, "reverse", lespl_libstr_reverse);
	box_set(s, "reversor",
			lespl_create_callable(mm, lespl_libstr_reverse));

	lespl_add_method(s, "concatenate", lespl_libstr_concat);
	box_set(s, "concatenator",
			lespl_create_callable(mm, lespl_libstr_concat));

	lespl_add_method(s, "length", lespl_libstr_length);
	box_set(s, "length_getter",
			lespl_create_callable(mm, lespl_libstr_length));

	lespl_add_method(s, "get", lespl_libstr_get);
	box_set(s, "getter", lespl_create_callable(mm, lespl_libstr_get));
	return s;
}

Box* lespl_libstr_new(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__str_value__"));
	Box* c = lespl_create_callable(box_mm(ctx), lespl_libstr_new_);
	box_set(c, "__prev_str__", arg);
	return c;
}

Box* lespl_libstr_new_(Box* clb, Box* arg, Box* ctx) {
	assert(box_has(clb, "__prev_str__"));
	Box* s = box_clone(box_get(clb, "__prev_str__"));
	box_set(s, "__str_value__", strdup(lespl_peel(arg)));
	return s;
}

Box* lespl_libstr_reverse(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	char* val = box_get(arg, "__str_value__");
	int l = strlen(val);
	char* n = strdup(val);
	for (int i = 0; i < l; i++) n[i] = val[l - i - 1];
	Box* s = box_clone(arg);
	box_set(s, "__str_value__", n);
	return s;
}

Box* lespl_libstr_concat(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__str_value__"));
	Box* c = lespl_create_callable(box_mm(ctx), lespl_libstr_concat_);
	box_set(c, "__prev_str__", arg);
	return c;
}

Box* lespl_libstr_concat_(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__str_value__"));
	char* val = box_get(arg, "__str_value__");
	assert(box_has(clb, "__prev_str__"));
	Box* prev_str = box_get(clb, "__prev_str__");
	char* prev_val = box_get(prev_str, "__str_value__");

	char* n = mm_alloc(box_mm(ctx), strlen(val) + strlen(prev_val) + 1,
			NULL, NULL);
	strcpy(n, prev_val);
	strcat(n, val);

	Box* s = box_clone(prev_str);
	box_set(s, "__str_value__", n);
	return s;
}

Box* lespl_libstr_length(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__str_value__"));
	char* val = box_get(arg, "__str_value__");
	return lespl_libint_create_from_int(box_mm(ctx), strlen(val));
}

Box* lespl_libstr_get(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__str_value__"));
	Box* c = lespl_create_callable(box_mm(ctx), lespl_libstr_get_);
	box_set(c, "__str__", arg);
	return c;
}

Box* lespl_libstr_get_(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__int_value__"));
	int64_t i = lespl_libint_unbox(arg);
	assert(box_has(clb, "__str__"));
	Box* str_b = box_get(clb, "__str__");
	char* str = box_get(str_b, "__str_value__");
	assert (i >= 0);
	assert (i < (int64_t) strlen(str));
	char* n = mm_alloc(box_mm(ctx), 2, NULL, NULL);
	n[0] = str[i]; n[1] = '\0';
	Box* s = box_clone(str_b);
	box_set(s, "__str_value__", n);
	return s;
}

