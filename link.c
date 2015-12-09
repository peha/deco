#include "arch.h"
#include "deco.h"
#include "extr.h"
#include "fs.h"
#include "link.h"
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
#include <unistd.h>


static void complete_first_link(struct archive *a)
	{
	size_t ldot, ldir;
	char *old = a->links.file.e;

	/* we are finally in possession of the sandbox name needed to build the link path */
	a->links.file.p = alloc(plen(a->sandbox) + 1 + elen(a->links.file) + 1);
	a->links.file.e = str_cp(str_cp(a->links.file.p, a->sandbox.p), "/");
	a->links.file.z = str_cp(a->links.file.e, old);
	free(old);

	/* build the archive link target */
	ldot = Absolute || a->file.p[0] != '/' ? strlen("../") : 0;
	ldir = Absolute ? 0 : dlen(a->file);
	a->links.target.p = alloc(ldot + ldir + elen(a->file) + 1);
	a->links.target.e = str_cp_max(ldir, str_cp_max(ldot, a->links.target.p, "../"), a->file.p);
	a->links.target.z = str_cp(a->links.target.e, a->file.e);
	}

static bool is_aux(const struct archive *a, const char *e)
	{
	return strncmp(a->links.file.e, e, a->lname) == 0                    /* same main name */
	       && strcasecmp(a->links.file.e + a->lname, e + a->lname) != 0  /* different exten */
	       && match_extr(a->extr, e) == a->lname;                        /* same match pos */
	}

static void add_link(struct archive *a, const char *e)
	{
	struct link *l;
	size_t se = strlen(e) + 1;

	l = alloc(sizeof(struct link));
	l->next = a->links.next;
	a->links.next = l;

	l->file.p = alloc(dlen(a->links.file) + se);
	l->file.e = str_cp_max(dlen(a->links.file), l->file.p, a->links.file.p);
	l->file.z = str_cp_tr(tolower, str_cp_max(a->lname, l->file.e, e), e + a->lname);
	l->target.p = alloc(dlen(a->links.target) + se);
	l->target.e = str_cp_max(dlen(a->links.target), l->target.p, a->links.target.p);
	l->target.z = str_cp(l->target.e, e);
	}

static bool make_symlink(struct link *l)
	{
	struct stat st;

	if (symlink(l->target.p, l->file.p) != 0)
		return err(MSG_SYMLINK, errno, l->file.p);
	if (lstat(l->file.p, &st) != 0)
		return err(MSG_STAT, errno, l->file.p);
	l->inode = st.st_ino;
	l->ctime = st.st_ctime;
	return true;
	}

bool create_links(struct archive *a)
	{
	DIR *dir;
	struct link *l;
	struct dirent *ent;
	char bak;

	complete_first_link(a);

	if (dlen(a->file) == 0)
		dir = open_dir(".");
	else
		{
		bak = a->file.e[0];
		a->file.e[0] = '\0';
		dir = open_dir(a->file.p);
		a->file.e[0] = bak;
		}
	if (!dir)
		return false;
	while ((ent = readdir(dir)))
		if (is_aux(a, ent->d_name))
			add_link(a, ent->d_name);
	closedir(dir);

	for (l = &a->links; l; l = l->next)
		if (!make_symlink(l))
			return false;
	return true;
	}

static bool del_link(const struct link *l)
	{
	struct stat st;
	ssize_t lread;
	char *target, *msg = NULL;

	if (lstat(l->file.p, &st) != 0)
		return errno == ENOENT /* deleted */ || err(MSG_STAT, errno, l->file.p);
	if (l->inode != st.st_ino || l->ctime != st.st_ctime /* replaced */)
		return true;

	target = alloc(plen(l->target) + 1);
	lread = readlink(l->file.p, target, plen(l->target) + 1);
	if (lread == -1)
		{
		if (errno != EINVAL)
			msg = MSG_READLINK;
		}
	else if ((size_t)lread == plen(l->target)
	         && memcmp(target, l->target.p, lread) == 0
	         && unlink(l->file.p) != 0
	         && errno != ENOENT)
		msg = MSG_DEL;
	free(target);
	return !msg || err(msg, errno, l->file.p);
	}

bool del_links(const struct archive *a)
	{
	const struct link *l;
	bool success = true;

	for (l = &a->links; l; l = l->next)
		if (!del_link(l))
			success = false;
	return success;
	}

void free_links(const struct archive *a)
	{
	struct link *l, *next;

	if (a->links.file.p /* we called complete_first_link() previously */)
		{
		for (l = (struct link *)&a->links; l; l = l->next)
			{
			free(l->file.p);
			free(l->target.p);
			}
		for (l = a->links.next; l; l = next)
			{
			next = l->next;
			free(l);
			}
		}
	else
		free(a->links.file.e);
	}
