// libint - a library providing integers in lespl.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#ifndef __LESPL_LIBINT_H
#define __LESPL_LIBINT_H

#include "../liblespl.h"

Box* lespl_libint_main(Box* clb, Box* arg, Box* ctx);
Box* lespl_libint_new(Box* clb, Box* arg, Box* ctx);
Box* lespl_libint_negate(Box* clb, Box* arg, Box* ctx);
Box* lespl_libint_add(Box* clb, Box* arg, Box* ctx);
Box* lespl_libint_subtract(Box* clb, Box* arg, Box* ctx);
Box* lespl_libint_equals(Box* clb, Box* arg, Box* ctx);

Box* lespl_libint_create_from_int(MM* mm, int64_t i);
Box* lespl_libint_create_from_string(MM* mm, const char* string);
int64_t lespl_libint_unbox(Box* lespl_int);

#endif
