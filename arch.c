#include "arch.h"
#include "deco.h"
#include "exec.h"
#include "extr.h"
#include "fs.h"
#include "link.h"
#include "math.h"
#include "mem.h"
#include "msg.h"
#include "str.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


static bool setup(struct archive *a)
	{
	size_t lexten = 0;

	switch (file_type(true, a->file.p))
		{
		case Unknown: return false;
		case None:    return err(MSG_NOT_EXIST, 0, a->file.p);
		case Dir:     return err(MSG_IS_DIR,    0, a->file.p);
		}
	a->file = build_path(a->file.p);

	if (Extension)
		{
		lexten = strlen(Extension);
		if (elen(a->file) > lexten + 1
		    && a->file.z[-lexten-1] == '.'
		    && strcasecmp(a->file.z - lexten, Extension) == 0)
			lexten = 0;
		}
	a->links.file.e = alloc(elen(a->file) + !!lexten + lexten + 1);
	a->links.file.z = str_cp_max(lexten, str_cp_max(lexten, str_cp(a->links.file.e, a->file.e), "."), Extension);

	find_extr(a);
	if (!a->extr)
		return err(MSG_FIND_EXTR, 0, a->file.p);
	str_tr(tolower, a->links.file.e + a->lname);
	return true;
	}

static char *prefix_num(unsigned int try, char *buf, const char *e, size_t le)
	{
	switch (try)
		{
		case 1: buf = str_cp(buf, "1-");
		case 0: return str_cp_max(le, buf, e);
		}
	buf[0] = '0' + try;
	return buf + 2 + le;
	}

static bool create_sandbox(struct archive *a)
	{
	size_t ldir;
	unsigned int i;

	ldir = Absolute ? dlen(a->file) : 0;
	a->sandbox.p = alloc(ldir + 2 + a->lname + 1);
	a->sandbox.e = str_cp_max(ldir, a->sandbox.p, a->file.p);

	for (i = 0; i < Tries; ++i)
		{
		a->sandbox.z = prefix_num(i, a->sandbox.e, a->links.file.e, a->lname);
		if (mkdir(a->sandbox.p, S_IRWXU | S_IRWXG | S_IRWXO) == 0)
			return true;
		if (errno != EEXIST)
			break;
		}
	return err(MSG_MKDIR, errno, a->sandbox.p);
	}

static void exec_extr(void *args)
	{
	struct archive *a = args;
	char *cmd[] = { a->extr->extract, a->links.file.e - 2, NULL }, *name;

	if (chdir(a->sandbox.p) != 0)
		die(MSG_CHDIR, errno, a->sandbox.p);
	name = alloc(strlen("Name=") + a->lname + 1);
	str_cp_max(a->lname, str_cp(name, "Name="), a->links.file.e);
	if (!put_env(name))
		exit(EXIT_FAILURE);
	cmd[1][0] = '.';
	exec(cmd);
	}

static bool norm_perms(const struct archive *a)
	{
	const char *args[] = { "chmod", "-R", "a=,+rwX", a->sandbox.p, NULL };

	return !a->extr->perms || spawn(exec, args, MSG_NORM_PERMS, a->sandbox.p);
	}

static int inc_rename(unsigned int tries, const char *src, enum type typ,
                      char *dst, size_t offset, const struct path ent)
	{
	unsigned int i;

	if (typ > None)
		for (i = 0; i < tries; ++i)
			{
			prefix_num(i, dst + offset, ent.e, elen(ent));
			switch (safe_rename(src, typ, dst))
				{
				case -1: return 0;
				case  1: return i + 1;
				}
			}
	return 0;
	}

static char *move_lower(const struct path dir, const struct path only, enum type *typ)
	{
	/*
	 * rename parent/last/only to parent/only (or parent/n-only)
	 * rmdir parent/last == dir
	 * try to remove n- prefix (or lower n)
	 */

	unsigned int try;
	char *p, *q;

	p = alloc(plen(dir) + 1 + elen(only) + 1);     /* plen(dir) + 1 >= ... */
	str_cp(str_cp(str_cp(p, dir.p), "/"), only.e);
	q = alloc(dlen(dir) + 2 + elen(only) + 1);     /* ... dlen(dir) + 2 */
	str_cp_max(dlen(dir), q, dir.p);

	*typ = file_type(false, p);
	try = inc_rename(Tries + 1, p, *typ, q, dlen(dir), only);
	if (!try)
		{
		err(MSG_RENAME, errno, p);
		free(p);
		free(q);
		return NULL;
		}
	if (rmdir(dir.p) != 0)
		err(MSG_DEL, errno, dir.p);
	try = inc_rename(MIN(try - 1, Tries), q, *typ, p, dlen(dir), only);
	free(try ? q : p);
	return try ? p : q;
	}

static bool handle_contents(const struct archive *a)
	{
	struct path first;
	enum type typ;
	DIR *dir;
	struct dirent *ent;
	char *moved;
	unsigned int entries = 0;

	dir = open_dir(a->sandbox.p);
	if (!dir)
		return false;
	while ((ent = readdir(dir)))
		{
		if (is_dot(ent->d_name))
			continue;
		if (++entries > 1 || a->extr->subdir)
			break;
		first.p = first.e = alloc(strlen(ent->d_name) + 1);
		first.z = str_cp(first.e, ent->d_name);
		}
	closedir(dir);

	if (entries == 0)
		{
		err(MSG_EMPTY, 0, a->file.p);
		return rmdir(a->sandbox.p) == 0 || err(MSG_DEL, errno, a->sandbox.p);
		}
	if (entries == 1 && !a->extr->subdir)
		{
		moved = move_lower(a->sandbox, first, &typ);
		free(first.p);
		if (moved)
			{
			pretty_print(moved, typ);
			free(moved);
			return true;
			}
		return false;
		}
	/* entries > 1 || a->extr->subdir */
	if (!a->extr->subdir)
		free(first.p);
	pretty_print(a->sandbox.p, Dir);
	return true;
	}

static bool del_arch(const struct archive *a)
	{
	struct path file;
	size_t longest = 0;
	const struct link *l;
	bool success = true;

	if (!Unlink)
		return true;

	for (l = &a->links; l; l = l->next)
		longest = MAX(longest, elen(l->target));
	file.p = alloc(dlen(a->file) + longest + 1);
	file.e = str_cp_max(dlen(a->file), file.p, a->file.p);
	for (l = &a->links; l; l = l->next)
		{
		str_cp(file.e, l->target.e);
		if (unlink(file.p) != 0)
			success = err(MSG_DEL, errno, file.p);
		}
	free(file.p);
	return success;
	}

bool extract(const char *arg)
	{
	struct archive a = { (char *)arg };
	bool success = false, ex;

	if (setup(&a))
		{
		if (create_sandbox(&a))
			if (create_links(&a)
			    && ((ex = spawn(exec_extr, &a, MSG_EXTRACT, a.file.p)) || !Clean)
			    && del_links(&a)
			    && norm_perms(&a)
			    && handle_contents(&a))
				success = ex && del_arch(&a);
			else
				rm_rf(a.sandbox.p);
		free(a.sandbox.p);  /* allocated by create_sandbox() */
		}
	free_links(&a);  /* allocated by setup() and create_links() */
	return success;
	}
