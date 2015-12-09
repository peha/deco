#include <sys/types.h>


char *str_cp(char *dst, const char *src);
char *str_cp_max(size_t n, char *dst, const char *src);
char *str_cp_tr(int (*tr)(int), char *dst, const char *src);


#ifndef STR
#define STR
static inline char *str_tr(int (*tr)(int), char *s)
	{
	return str_cp_tr(tr, s, s);
	}
#endif
