// libbool - a library providing booleans in lespl.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#ifndef __LESPL_LIBBOOL_H
#define __LESPL_LIBBOOL_H

#include "../liblespl.h"

Box* lespl_libbool_main(Box* clb, Box* arg, Box* ctx);
Box* lespl_libbool_true(Box* clb, Box* arg, Box* ctx);
Box* lespl_libbool_false(Box* clb, Box* arg, Box* ctx);
Box* lespl_libbool_new(Box* clb, Box* arg, Box* ctx);
Box* lespl_libbool_not(Box* clb, Box* arg, Box* ctx);
Box* lespl_libbool_and(Box* clb, Box* arg, Box* ctx);
Box* lespl_libbool_or(Box* clb, Box* arg, Box* ctx);
Box* lespl_libbool_xor(Box* clb, Box* arg, Box* ctx);

Box* lespl_libbool_create(MM* mm, bool i);
Box* lespl_libbool_inverted(Box* lespl_bool);
bool lespl_libbool_unbox(Box* lespl_bool);

#endif
