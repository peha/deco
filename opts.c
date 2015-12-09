#include "opts.h"

#include <stdbool.h>
#include <stdlib.h>


char **parse_opts(char **argv, const struct option *opts)
	{
	const struct option *opt;
	unsigned int chr;

	while (*++argv)
		{
		/* non-option argument? */
		if ((*argv)[0] != '-' || !(*argv)[1])
			break;

		/* option delimiter? */
		if ((*argv)[1] == '-' && !(*argv)[2])
			{
			++argv;
			break;
			}

		/*
		 * For now, each option taking an argument is only allowed to occur
		 * once. We accept the -x arg style, but reject the deprecated -xarg
		 * and -yzxarg styles.
		 */
		for (chr = 1; (*argv)[chr]; ++chr)
			{
			for (opt = opts; opt->key; ++opt)
				if ((*argv)[chr] == opt->key)
					if (opt->args)
						{
						if (chr == 1
						    && !(*argv)[2]
						    && !*(char **)opt->var
						    && argv[1])
							{
							*(char **)opt->var = *++argv;
							goto nextarg;
							}
						}
					else
						{
						*(bool *)opt->var = true;
						goto nextchr;
						}
			return NULL;
			nextchr: ;
			}
		nextarg: ;
		}

	return argv;
	}
