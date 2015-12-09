#include "arch.h"
#include "deco.h"
#include "extr.h"
#include "fs.h"
#include "mem.h"
#include "msg.h"
#include "str.h"

#include <dirent.h>
#include <errno.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>


struct extractor *Extractors;


regoff_t match_extr(struct extractor *x, const char *file)
	{
	regmatch_t m;

	return (regexec(&x->matcher, file, 1, &m, 0) == 0
	        && m.rm_so > 0
	        && file[--m.rm_so] == '.') ? m.rm_so : -1;
	}

void find_extr(struct archive *a)
	{
	/* caller ensures a->extr == NULL */

	regoff_t pos;
	struct extractor *x;

	for (x = Extractors; x; x = x->next)
		{
		pos = match_extr(x, a->links.file.e);
		if (pos > 0
		    && (!a->extr || pos < a->lname)
		    && (!Extension || strcasecmp(Extension, a->links.file.e + pos + 1) == 0))
			{
			a->extr = x;
			a->lname = pos;
			if (Extension)
				return;
			}
		}
	}

void add_extrs(const char *p)
	{
	struct path cfg;
	size_t lp;
	DIR *dir;
	struct dirent *ent;
	struct extractor *x;

	dir = opendir(p);
	if (!dir)
		return;

	lp = strlen(p);
	while ((ent = readdir(dir)))
		{
		if (is_dot(ent->d_name))
			continue;

		x = alloc(sizeof(struct extractor));

		cfg.p = alloc(lp + 1 + strlen(ent->d_name) + sizeof "/subdirectory");
		cfg.e = str_cp(str_cp(str_cp(str_cp(cfg.p, p), "("), ent->d_name), ")$") - 1;

		if (regcomp(&x->matcher, cfg.p + lp, REG_EXTENDED | REG_ICASE) != 0)
			{
			err(MSG_REGCOMP, errno, cfg.p + lp);
			free(cfg.p);
			free(x);
			continue;
			}

		cfg.p[lp] = cfg.e[-1] = '/';

		str_cp(cfg.e, "permissions");
		x->perms = file_type(true, cfg.p) > None;
		str_cp(cfg.e, "subdirectory");
		x->subdir = file_type(true, cfg.p) > None;
		str_cp(cfg.e, "extract");
		x->extract = cfg.p;

		x->next = Extractors;
		Extractors = x;
		}

	closedir(dir);
	}
