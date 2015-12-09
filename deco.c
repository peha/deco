#include "arch.h"
#include "exec.h"
#include "extr.h"
#include "mem.h"
#include "msg.h"
#include "opts.h"
#include "str.h"

#include <stdbool.h>
#include <stdlib.h>


const unsigned char Tries = 5;  /* values 1 - 9 are allowed */
bool Absolute, Clean, Unlink, Verbose;
char *Extension, **Archives;


static bool setup_opts(char **argv)
	{
	static const struct option opts[] =
		{
		'a', 0, &Absolute,
		'c', 0, &Clean,
		'e', 1, &Extension,
		'u', 0, &Unlink,
		'v', 0, &Verbose,
		0
		};

	Archives = parse_opts(argv, opts);
	return (Archives && *Archives) || (WRITE_STR(STDERR_FILENO, MSG_USAGE), false);
	}

#define BOOL_TO_ENV(b, e) ((b) ? put_env(e "=") : (unsetenv(e), true))

static bool setup_env(void)
	{
	/*
	 * Setting POSIXLY_CORRECT prevents GNU getopt (possibly used by external
	 * programs) from hallucinating options after non-option arguments.
	 */

	return BOOL_TO_ENV(true, "POSIXLY_CORRECT")
	       && BOOL_TO_ENV(Verbose, "Verbose")
	       && BOOL_TO_ENV(!Clean, "Keep");
	}

static bool setup_extrs(void)
	{
	char *home, *local;

	/* last added extractors enjoy priority */
	add_extrs(SHARE);
	home = getenv("HOME");
	if (home)
		{
		local = alloc(strlen(home) + sizeof "/.deco");
		str_cp(str_cp(local, home), "/.deco");
		add_extrs(local);
		free(local);
		}
	return Extractors || err(MSG_NO_EXTRS, 0, NULL);
	}

int main(int argc, char **argv)
	{
	int status = EXIT_SUCCESS;

	if (!setup_opts(argv) || !setup_env() || !setup_extrs())
		exit(EXIT_FAILURE);
	for (; *Archives; ++Archives)
		if (!extract(*Archives))
			status = EXIT_FAILURE;
	exit(status);
	}
