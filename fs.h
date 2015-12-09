#include <dirent.h>
#include <stdbool.h>
#include <sys/types.h>


#ifndef FS
#define FS
struct path
	{
	char *p, *e, *z;
	};

static inline size_t plen(struct path x)
	{
	return x.z - x.p;
	}

static inline size_t dlen(struct path x)
	{
	return x.e - x.p;
	}

static inline size_t elen(struct path x)
	{
	return x.z - x.e;
	}

static inline bool is_dot(const char *e)
	{
	return e[0] == '.' && (!e[1] || (e[1] == '.' && !e[2]));
	}

enum type { Unknown, None, Dir, Other };
#endif


struct path build_path(const char *p);
enum type file_type(bool follow, const char *p);
DIR *open_dir(const char *p);
void pretty_print(const char *p, enum type typ);
bool rm_rf(const char *p);
int safe_rename(const char *src, enum type typ, const char *dst);
