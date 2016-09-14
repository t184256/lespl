// libskip - a library providing argument ignoring in lespl.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#ifndef __LIBSKIP_H
#define __LIBSKIP_H

#include "../liblespl.h"

Box* lespl_libskip_main(Box* clb, Box* arg, Box* ctx);
Box* lespl_libskip_skippy(Box* next);
Box* lespl_libskip_ignore(Box* clb, Box* arg, Box* ctx);

#endif
