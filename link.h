#include "fs.h"

#include <stdbool.h>
#include <sys/types.h>


#ifndef LINK
#define LINK
struct link
	{
	struct path file, target;
	ino_t inode;
	time_t ctime;
	struct link *next;
	};
#endif

struct archive;


bool create_links(struct archive *a);
bool del_links(const struct archive *a);
void free_links(const struct archive *a);
