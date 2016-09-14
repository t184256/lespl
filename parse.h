// Parser - a lespl core syntax parser.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

// Parses a const char* of "a b ( cd ( e f g ) ( ) )" into a tree of boxes
// (see box.h), which could be written like this:
// {
//   "__source_tree__": none,
//   "__code__": "a",
//   "__code__": "b",
//   "__box__": {
//     "__source_tree__": none,
//     "__code__": "cd",
//     "box": {"__source_tree__": none, "code": "e", "code": "f", "code": "g"},
//     "box": {"__source_tree__": none},
//   }
// }

#ifndef __PARSE_H
#define __PARSE_H

#include "common.h"
#include "box.h"
#include "mm.h"

Box* lespl_parse(const char* code, MM* optional_mm);

void lespl_debug_dump_src(FILE* f, Box* s, uint depth);

#endif
