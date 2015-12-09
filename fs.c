#include "exec.h"
#include "fs.h"
#include "msg.h"

#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


struct path build_path(const char *p)
	{
	struct path r = { (char *)p, (char *)p, (char *)p };

	while (*r.z)
		if (*r.z++ == '/')
			r.e = r.z;
	return r;
	}

enum type file_type(bool follow, const char *p)
	{
	struct stat st;

	if ((follow ? stat : lstat)(p, &st) == 0)
		return S_ISDIR(st.st_mode) ? Dir : Other;
	return errno == ENOENT ? None : (err(MSG_STAT, errno, p), Unknown);
	}

DIR *open_dir(const char *p)
	{
	DIR *dir;

	dir = opendir(p);
	if (!dir)
		err(MSG_OPENDIR, errno, p);
	return dir;
	}

void pretty_print(const char *p, enum type typ)
	{
	WRITE_STR(STDOUT_FILENO, p);
	if (typ == Dir)
		WRITE_STR(STDOUT_FILENO, "/");
	WRITE_STR(STDOUT_FILENO, "\n");
	}

bool rm_rf(const char *p)
	{
	const char *args[] = { "rm", "-rf", "--", p, NULL };

	return spawn(exec, args, MSG_DEL, p);
	}

int safe_rename(const char *src, enum type typ, const char *dst)
	{
	/* caller ensures src and dst don't refer to the same path */

	if ((typ == Dir ? mkdir(dst, S_IRWXU | S_IRWXG | S_IRWXO): link(src, dst)) != 0)
		return errno == EEXIST ? 0 : (err(typ == Dir ? MSG_MKDIR : MSG_LINK, errno, dst), -1);
	if ((typ == Dir ? rename(src, dst) : unlink(src)) == 0)
		return 1;
	err(typ == Dir ? MSG_RENAME : MSG_DEL, errno, src);
	if ((typ == Dir ? rmdir : unlink)(dst) != 0)
		err(MSG_DEL, errno, dst);
	return 0;
	}
