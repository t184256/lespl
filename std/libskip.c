// libskip - a library providing argument ignoring in lespl.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#include "libskip.h"

static Box* lespl_libskip_skippy_(Box* clb, Box* arg, Box* ctx);

Box* lespl_libskip_main(Box* clb, Box* unused_arg, Box* ctx) {
	return lespl_create_callable(box_mm(ctx), lespl_libskip_ignore);
}

Box* lespl_libskip_skippy(Box* next) {
	Box* skpy = lespl_create_callable(box_mm(next), lespl_libskip_skippy_);
	box_set(skpy, "__skippy_next__", next);
	return skpy;
}

static Box* lespl_libskip_skippy_(Box* clb, Box* skipped_arg, Box* ctx) {
	assert(box_has(clb, "__skippy_next__"));
	return box_get(clb, "__skippy_next__");
}

Box* lespl_libskip_ignore(Box* clb, Box* arg, Box* ctx) {
	return NULL;
}
