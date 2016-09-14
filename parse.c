// Parser - a lespl core syntax parser.
// Part of lespl (lame educational simple programming language)
// by Alexander Sosedkin <monk@unboiled.info>, see LICENSE file.

#include "parse.h" // Please refer to it for a general overview of parse

static Box* lespl_parse_(const char** p, MM* optional_mm);
static char* lespl_parse_read_token(const char** p_);

Box* lespl_parse(const char* code, MM* optional_mm) {
	const char* p = code;
	return lespl_parse_(&p, optional_mm ? optional_mm : mm_create());
}

static Box* lespl_parse_(const char** p_, MM* mm) {
	char* token;
	Box* child;
	Box* box = box_create(mm);
	box_add(box, "__source_tree__", box_create(mm));
	while (**p_) {
		token = lespl_parse_read_token(p_);
		if (!strcmp(token, "(")) {
			child = lespl_parse_(p_, mm);
			box_add(box, "__box__", child);
			free(token);
			continue;
		}
		if (!strcmp(token, ")")) {
			free(token);
			break;
		}
		box_add(box, "__code__", strdup(token));
		free(token);
	}
	return box;
}

static char* lespl_parse_read_token(const char** p_) {
	const char* p = *p_;
	char* result;
	uint tcap = 2, tsize = 0;
	char* token = malloc(tcap);
	while (*p && !isblank(*p) && *p != '\n') {
		if (tsize >= tcap - 1) {
			tcap *= 2;
			token = realloc(token, tcap);
			if (!token) MEM_DIE;
		}
		token[tsize++] = *p++;
	}
	token[tsize] = '\0';
	while (*p && (isblank(*p) || *p == '\n'))
		p++;
	result = strdup(token);
	free(token);
	*p_ = p;
	return result;
}

void lespl_debug_dump_src(FILE* f, Box* s, uint depth) {
	Box* child;
	uint i, j;
	assert(!strcmp(box_nth_key(s, 0), "__source_tree__"));
	for (i = 1; i < s->size; i++) {
		for (j = 0; j < depth; j++)
			putchar('#');
		if (!strcmp(box_nth_key(s, i), "__code__")) {
			fprintf(f, " '%s'\n", (char*) box_nth(s, i));
		} else {
			assert(!strcmp(box_nth_key(s, i), "__box__"));
			child = box_nth(s, i);
			fprintf(f, "[%u]:\n", box_size(child));
			lespl_debug_dump_src(f, child, depth + 1);
		}
	}
}

