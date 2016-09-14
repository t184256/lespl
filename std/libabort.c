// libabort - a library providing an instant interpreter about in lespl.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#include "../liblespl.h"

Box* lespl_libabort_main(Box* clb, Box* arg, Box* ctx);

Box* lespl_libabort_main(Box* clb, Box* unused_arg, Box* ctx) {
	exit(EXIT_FAILURE);
}

