// libcontext - a library providing context manipulation in lespl.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#include "../liblespl.h"

Box* lespl_libcontext_main(Box* clb, Box* arg, Box* ctx);
Box* lespl_libcontext_set(Box* clb, Box* arg, Box* ctx);
Box* lespl_libcontext_set_(Box* clb, Box* arg, Box* ctx);

Box* lespl_libcontext_main(Box* clb, Box* unused_arg, Box* ctx) {
	MM* mm = box_mm(ctx);
	Box* module = box_create(mm);
	box_set(module, "set",
			lespl_create_callable(mm, lespl_libcontext_set));
	return module;
}

Box* lespl_libcontext_set(Box* clb, Box* arg, Box* ctx) {
	// TODO: str? trick
	Box* c = lespl_create_callable(box_mm(ctx), lespl_libcontext_set_);
	box_set(c, "__context_var_name__", strdup(lespl_peel(arg)));
	return c;
}

Box* lespl_libcontext_set_(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(clb, "__context_var_name__"));
	box_set(ctx, box_get(clb, "__context_var_name__"), arg);
	return NULL;
}

