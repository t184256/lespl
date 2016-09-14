// librestart - a library providing a tail recursion special object in lespl.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#include "../liblespl.h"
#include "libbool.h"

Box* lespl_librestart_main(Box* clb, Box* arg, Box* ctx);
Box* lespl_librestart_decide(Box* clb, Box* arg, Box* ctx);

Box* lespl_librestart_main(Box* clb, Box* unused_arg, Box* ctx) {
	return lespl_create_callable(box_mm(ctx), lespl_librestart_decide);
}

Box* lespl_librestart_decide(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__bool_value__"));
	if (!lespl_libbool_unbox(arg)) return NULL;
	Box* restarter = box_create(box_mm(ctx));
	box_add(restarter, "__restart__", NULL);
	return restarter;
}

