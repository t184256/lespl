// libio - a library providing input/output in lespl.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#include "../liblespl.h"
#include "libbool.h"
#include "libint.h"
#include <stdio.h>

Box* lespl_libio_main(Box* clb, Box* arg, Box* ctx);
Box* lespl_libio_print(Box* clb, Box* arg, Box* ctx);

Box* lespl_libio_main(Box* clb, Box* unused_arg, Box* ctx) {
	MM* mm = box_mm(ctx);

	Box* module = box_create(mm);

	Box* box_stdout = box_create(mm);
	box_set(box_stdout, "print",
			lespl_create_callable(mm, lespl_libio_print));

	box_set(module, "stdout", box_stdout);
	return module;
}

Box* lespl_libio_print(Box* clb, Box* arg, Box* ctx) {
	arg = lespl_eval_tree(arg, box_clone(ctx));
	if (!arg) { printf("<ident>\n"); return NULL; }
	if (box_has(arg, "__str_value__")) {
		printf("%s\n", (char* )box_get(arg, "__str_value__"));
		return NULL;
	}
	if (box_has(arg, "__int_value__")) {
		printf("%lld\n", (long long int) lespl_libint_unbox(arg));
		return NULL;
	}
	if (box_has(arg, "__bool_value__")) {
		printf(lespl_libbool_unbox(arg) ? "true\n" : "false\n");
		return NULL;
	}
	lespl_dir(stdout, "<some lespl object:", arg, ">\n");
	return NULL;
}
