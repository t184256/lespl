#include "liblespl.h"

int main(void) {
	Box* ctx = lespl_default_context(NULL);
	Box* result = lespl_eval_file(stdin, ctx);

//	lespl_cleanup_except(result);
//	assert(!box_size(result));
	assert(!result);
	lespl_cleanup_all(ctx);

	return EXIT_SUCCESS;
}
