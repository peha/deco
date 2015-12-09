#include "fs.h"
#include "link.h"

#include <regex.h>
#include <stdbool.h>


#ifndef ARCHIVE
#define ARCHIVE
struct archive
	{
	struct path file, sandbox;
	struct link links;
	struct extractor *extr;
	regoff_t lname;
	};
#endif


bool extract(const char *arg);
