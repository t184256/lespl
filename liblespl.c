// liblespl - a library for embedding lespl interpreter
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#include "liblespl.h"

typedef struct {
	LesplFunction func;
	void* libhandle; // a handle for the dynamic library it resides in
} LesplFunctionPointer;

static Box* lespl_apply_(Box* curr, Box* next, Box* context);
static void lespl_finalize_lfp(void* lfp_);
static LesplFunctionPointer* lespl_create_function(MM* mm, LesplFunction func,
		void* optional_libhandle);
static LesplFunction lespl_dload(const char* libname, const char* fname,
		void** library_handle_ptr);
static FILE* lespl_find_native_module_file(const char* lname);

// A simplest interface to lespl: evaluates a piece of code (char*)
Box* lespl_eval(const char* source_code, Box* optional_context) {
	Box* context = optional_context;
	if (!context) context = lespl_default_context(NULL);
	context = box_clone(context);

	Box* source_tree = lespl_parse(source_code, box_mm(context));

	return lespl_eval_tree(source_tree, context); // !!!
} // After that please do lespl_cleanup_all(result) when you are done.

// The real interpreter function: evaluates a source tree
Box* lespl_eval_tree(Box* source, Box* context) {
	if (!source) return NULL;
	assert(box_mm(source) == box_mm(context));
	MM* mm = box_mm(source);
	//if (!box_size(source)) return box_create(box_mm(source)); // () -> None
	if (!box_has(source, "__source_tree__")) return source;
	Box* curr = NULL;
	Box* next;
	for (unsigned int i = 1; i < box_size(source); i++) {
		char* next_name = box_nth_key(source, i);
		void* next_data = box_nth(source, i);
		if (!strcmp(next_name, "__code__")) {
			next = box_create(box_mm(source));
			box_add(next, "__source_tree__", box_create(mm));
			box_add(next, next_name, next_data);
		} else if (!strcmp(next_name, "__box__")) {
			// Force evaluation if after NULL, but not otherwise
			next = curr ? next_data : \
				lespl_eval_tree(next_data, box_clone(context));
		} else { dbg_out("BAD %s\n", next_name); exit(1); }
		curr = lespl_apply_(curr, next, context);

		// A trick that enables stackless tail recursion
		if (curr)
			if (box_has(curr, "__restart__"))
				i = 1 - 1;
	}
	//if (!curr) return box_create(box_mm(source)); // NULL -> None
	return curr;
}

// Deprecate in favor of apply when the code will be debugged enough
Box* lespl_apply_(Box* curr, Box* next, Box* context) {
	//assert(box_size(next) == 1);
	if (next) assert(box_mm(next) == box_mm(context));
	if (next && curr) assert(box_mm(next) == box_mm(curr));
	if (curr) assert(curr != next);
	//dbg_out("next '%s' %p\n", box_nth_key(next, 0), box_nth(next, 0));
	//dbg_out("curr b4 %p\n", curr);

	curr = lespl_apply(curr, next, context);

	//dbg_out("curr af %p\n", curr);
	return curr;
}

// Provides a context with a 'load' object to start with
Box* lespl_default_context(MM* optional_mm) {
	MM* mm = optional_mm ? optional_mm : mm_create();
	Box* ctx = box_create(mm);
	box_set(ctx, "load", lespl_create_callable(mm, lespl_load_module));
	return ctx;
}

// Does the most important thing: an application of one object to another
Box* lespl_apply(Box* curr, Box* next, Box* context) {
	//dbg_out("apply %p %p\n", curr, next);
	if (!next && !curr) return NULL;
	assert(next);
	MM* mm = box_mm(context);
	assert(mm == box_mm(next));

	if (!curr) {
		//dbg_out("no curr\n");
		if (box_has(next, "__code__")) {
			char* name = (char*) box_get(next, "__code__");
			//dbg_out("lookup %s in context\n", name);
			if (box_has(context, name)) {
				return (Box*) box_get(context, name);
			} else {
				fprintf(stderr, "! No '%s' in context\n", name);
				lespl_dir(stderr, "! context:", context, "\n");
				exit(1);
			}
		}
		return next;
	}
	//dbg_out("curr present\n");
	// TODO: this is where the magic should happen
	if (box_has(curr, "__call__")) {
		//dbg_out("curr __call__!\n");
		LesplFunctionPointer* lfp;
		lfp = box_get(curr, "__call__");
		assert(box_mm(curr) == box_mm(next));
		assert(box_mm(curr) == box_mm(context));
		return lfp->func(curr, next, context);
	}
	//dbg_out("curr not callable\n");
	if (box_has(next, "__code__")) {
		char* name = (char*) box_get(next, "__code__");
		//dbg_out("lookup %s in context\n", name);
		if (box_has(context, name))
			return (Box*) box_get(context, name);
		//dbg_out("lookup %s in curr\n", name);
		if (box_has(curr, name)) {
			//dbg_out("found %s in curr\n", name);
			//lespl_dir(stderr, "curr:", curr);
			Box* attr = box_get(curr, name);
			if (box_has(attr, "__meth__")) {
				//dbg_out("__meth__!\n");
				LesplFunctionPointer* lfp;
				lfp = box_get(attr, "__meth__");
				assert(box_mm(curr) == box_mm(attr));
				assert(box_mm(curr) == box_mm(context));
				return lfp->func(attr, curr, context);
			}
			return (Box*) box_get(curr, name);
		}
		fprintf(stderr, "! No '%s' in context or current\n", name);
		lespl_dir(stderr, "! context:", context, "\n");
		lespl_dir(stderr, "! current:", curr, "\n");
		exit(1);
	}
	dbg_out("cannot apply\n");
	exit(1); // TODO: better error handling (propagate NULLs on errors?)
	return NULL;
}

// Instructs garbage collector to clean everything unrelated to this object
void lespl_cleanup_except(Box* box) {
	box_cleanup_except(box);
}

// Finalizes and cleans everything managed by this object's mm
void lespl_cleanup_all(Box* box) {
	box_cleanup_all(box);
}

// Prints the keys of a box
void lespl_dir(FILE* f, const char* prefix, Box* box, const char* postfix) {
	if (box_size(box)) {
		fprintf(f, "%s", prefix);
		for (uint i = 0; i < box_size(box); i++)
			fprintf(f, " %s", box_nth_key(box, i));
		fprintf(f, "%s", postfix);
	}
}

// Creates a LesplFunctionPointer to be added into a box as __call__
LesplFunctionPointer* lespl_create_function(MM* mm, LesplFunction func,
		void* optional_libhandle) {
	LesplFunctionPointer* lfp = mm_alloc(mm, sizeof(LesplFunctionPointer),
			optional_libhandle ? lespl_finalize_lfp : NULL, NULL);
	lfp->func = func;
	lfp->libhandle = optional_libhandle;
	return lfp;
}

// dlcloses a dynamically loaded library specified in the LesplFunctionPointer
static void lespl_finalize_lfp(void* lfp_) {
	LesplFunctionPointer* lfp = lfp_;
	if (lfp->libhandle) {
		//dbg_out("dlclose %p\n", lfp->libhandle);
		dlclose(lfp->libhandle);
	}
}

// Hides the details of creating a callable from a C function
void lespl_make_callable(Box* box, LesplFunction func) {
	box_set(box, "__call__",
			lespl_create_function(box_mm(box), func, NULL));
}

// Hides the details of adding a C function as a method
void lespl_add_method(Box* box, const char* name, LesplFunction func) {
	Box* meth = box_create(box_mm(box));
	box_set(meth, "__meth__",
			lespl_create_function(box_mm(box), func, NULL));
	box_set(box, name, meth);
}

LesplFunction lespl_dload(const char* libname, const char* fname, void** lh) {
	char ln[128];
	sprintf(ln, "lib%s.so", libname);
	void* lib = dlopen(ln, RTLD_LAZY);
	if (!lib) { dbg_out("! Could not load %s\n", ln); exit(1); }
	char fn[128];
	sprintf(fn, "lespl_lib%s_%s", libname, fname);
	LesplFunction f = dlsym(lib, fn);
	if (!f) { dbg_out("! Could not find %s in %s\n", fn, ln); exit(1); }
	*lh = lib;
	return f;
}

// Like lespl_make_callable, but load the function from a shared library
void lespl_make_callable_dl(Box* box, const char* libname, const char* fname) {
	void* lib;
	LesplFunction f = lespl_dload(libname, fname, &lib);
	box_set(box, "__call__", lespl_create_function(box_mm(box), f, lib));
}

// Like lespl_add_method, but load the function from a shared library
void lespl_add_method_dl(Box* box,  const char* methodname,
		const char* libname, const char* funcname) {
	void* lib;
	LesplFunction f = lespl_dload(libname, funcname, &lib);
	Box* meth = box_create(box_mm(box));
	box_set(meth, "__meth__",
			lespl_create_function(box_mm(box), f, lib));
	box_set(box, methodname, meth);
}

// Like lespl_make_callable, but for a new box
Box* lespl_create_callable(MM* mm, LesplFunction func) {
	Box* n = box_create(mm);
	lespl_make_callable(n, func);
	return n;
}

// Like lespl_make_callable_dl, but for a new box
Box* lespl_create_callable_dl(MM* mm, const char* lname, const char* fname) {
	Box* n = box_create(mm);
	lespl_make_callable_dl(n, lname, fname);
	return n;
}

#define __LESPL_TRY_LOAD_NATIVE(fmtstr) sprintf(ln, fmtstr, lname); \
	file = fopen(ln, "r"); if (file) return file;
static FILE* lespl_find_native_module_file(const char* lname) {
	char ln[128];
	FILE* file = NULL;
	__LESPL_TRY_LOAD_NATIVE("lib%s.lespl");
	__LESPL_TRY_LOAD_NATIVE("std/lib%s.lespl");
	__LESPL_TRY_LOAD_NATIVE("/usr/lib/lespl/lib%s.lespl");
	return file;
}
#undef __LESPL_TRY_LOAD_NATIVE

Box* lespl_load_module_named(MM* mm, const char* lname) {
	//dbg_out(stderr, "* dynamically loading %s...\n", lname);
	FILE* native = lespl_find_native_module_file(lname);
	if (native) {
		Box* m = lespl_eval_file(native, lespl_default_context(mm));
		fclose(native);
		return m;
	}
	Box* m = lespl_create_callable_dl(mm, lname, "main");
	return lespl_apply(m, box_create(mm), box_create(mm)); // TODO: ctx?
}

Box* lespl_load_module(Box* clb, Box* arg, Box* ctx) {
	assert(box_size(arg) == 2);
	assert(box_has(arg, "__code__"));
	char* val = box_get(arg, "__code__");
	return lespl_load_module_named(box_mm(ctx), val);
}

Box* lespl_import_module(Box* clb, Box* arg, Box* ctx) {
	assert(box_size(arg) == 2);
	assert(box_has(arg, "__code__"));
	char* val = box_get(arg, "__code__");
	box_set(ctx, val, lespl_load_module_named(box_mm(ctx), val));
	return NULL;
}

Box* lespl_eval_file(FILE* source_file_pointer, Box* optional_context) {
	size_t size = 0, capacity = 32; // TODO: increase to 4096 later
	char* code = malloc(capacity);
	if (!code) MEM_DIE;
	char c;
	while ((c = fgetc(source_file_pointer)) != EOF) {
		code[size++] = c;
		if (size + 1 > capacity)
			if (!(code = realloc(code, capacity *= 2))) MEM_DIE;
	}
	code[size] = '\0';
	Box* result = lespl_eval(code, optional_context);
	free(code);
	return result;
}

Box* lespl_eval_filename(const char* filename, Box* optional_context) {
	FILE* file = fopen(filename, "r");
	Box* result = lespl_eval_file(file, optional_context);
	fclose(file);
	return result;
}

char* lespl_peel(Box* literal_or_string) {
	Box* los = literal_or_string;
	if (box_size(los) == 2 &&
			!strcmp(box_nth_key(los, 0), "__source_tree__") &&
			!strcmp(box_nth_key(los, 1), "__code__"))
		return box_get(los, "__code__");
	if (box_has(los, "__str_value__"))
		return box_get(los, "__str_value__");
	die("argument is neither string nor a literal");
}
