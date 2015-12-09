#include <regex.h>
#include <stdbool.h>


#ifndef EXTRACTOR
#define EXTRACTOR
struct extractor
	{
	regex_t matcher;
	char *extract;
	bool perms, subdir;
	struct extractor *next;
	};
#endif


extern struct extractor *Extractors;


regoff_t match_extr(struct extractor *x, const char *file);
void find_extr(struct archive *a);
void add_extrs(const char *p);
