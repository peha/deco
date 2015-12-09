#include "msg.h"

#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>


void *alloc(size_t size)
	{
	void *res;

	res = malloc(size);
	if (!res)
		die(MSG_ALLOC, errno, NULL);
	return res;
	}
