// libflow - a library providing flow control in lespl.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#include "libbool.h"
#include "libskip.h"

Box* lespl_libflow_main(Box* clb, Box* arg, Box* ctx);
Box* lespl_libflow_if(Box* clb, Box* arg, Box* ctx);
static Box* lespl_libflow_keepnskip_(Box* clb, Box* arg, Box* ctx);

Box* lespl_libflow_main(Box* clb, Box* unused_arg, Box* ctx) {
	MM* mm = box_mm(ctx);
	Box* m = box_create(mm);
	box_set(m, "if", lespl_create_callable(mm, lespl_libflow_if));
	return m;
}

Box* lespl_libflow_if(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	assert(box_has(arg, "__bool_value__"));
	return lespl_libbool_unbox(arg) ?
		lespl_create_callable(box_mm(ctx), lespl_libflow_keepnskip_) :
		lespl_create_callable(box_mm(ctx), lespl_libskip_ignore);
}

Box* lespl_libflow_keepnskip_(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	return lespl_libskip_skippy(arg);
}
