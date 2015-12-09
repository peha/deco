#include "msg.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


bool err(const char *msg, int syserr, const char *id)
	{
	WRITE_STR(STDERR_FILENO, "deco: ");
	WRITE_STR(STDERR_FILENO, msg);
	if (syserr)
		{
		WRITE_STR(STDERR_FILENO, " (");
		WRITE_STR(STDERR_FILENO, strerror(syserr));
		WRITE_STR(STDERR_FILENO, ")");
		}
	if (id)
		{
		WRITE_STR(STDERR_FILENO, ": ");
		WRITE_STR(STDERR_FILENO, id);
		}
	WRITE_STR(STDERR_FILENO, "\n");
	return false;  /* enables writing: return okay() || err(...) */
	}

void die(const char *msg, int syserr, const char *id)
	{
	err(msg, syserr, id);
	exit(EXIT_FAILURE);
	}
