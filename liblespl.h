// liblespl - a library for embedding lespl interpreter
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#ifndef __LESPL_H
#define __LESPL_H

#include "common.h"
#include "box.h"
#include "mm.h"
#include "parse.h"

// The part of the API you really need for embedding in C programs
Box* lespl_eval(const char* source_code, Box* optional_context);
void lespl_cleanup_except(Box* box);
void lespl_cleanup_all(Box* box);

// The less popular part of the API.
Box* lespl_eval_file(FILE* source_file_pointer, Box* optional_context);
Box* lespl_eval_filename(const char* filename, Box* optional_context);
Box* lespl_default_context(MM* optional_mm);
Box* lespl_apply(Box* curr, Box* next, Box* context);
Box* lespl_eval_tree(Box* source_tree, Box* context);

// Only needed to create or load custom functions
typedef Box* (*LesplFunction)(Box* clb, Box* arg, Box* ctx);
void lespl_make_callable(Box* box, LesplFunction func);
void lespl_make_callable_dl(Box* box, const char* libname, const char* fname);
void lespl_add_method(Box* box, const char* name, LesplFunction func);
void lespl_add_method_dl(Box* box,  const char* methodname,
		const char* libname, const char* funcname);

// Shortcuts for make_callable and make_callable_dl
Box* lespl_create_callable(MM* mm, LesplFunction func);
Box* lespl_create_callable_dl(MM* mm, const char* libname, const char* fname);

// Loading modules
Box* lespl_load_module_named(MM* mm, const char* lname);
Box* lespl_load_module(Box* clb, Box* arg, Box* ctx);
Box* lespl_import_module(Box* clb, Box* arg, Box* ctx);

// Utilities you don't need
void lespl_dir(FILE* f, const char* prefix, Box* box, const char* postfix);
char* lespl_peel(Box* literal_or_string);
#endif
