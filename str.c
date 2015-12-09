#include <sys/types.h>


char *str_cp(char *dst, const char *src)
	{
	while ((*dst = *src++))
		++dst;
	return dst;
	}

char *str_cp_max(size_t n, char *dst, const char *src)
	{
	/*
	 * Caller ensures dst has at least MIN(strlen(src), n) + 1 bytes space.
	 * src == NULL is okay if n == 0.
	 */

	while ((*dst = n-- ? *src++ : '\0'))
		++dst;
	return dst;
	}

char *str_cp_tr(int (*tr)(int), char *dst, const char *src)
	{
	while ((*dst = (char)tr((unsigned char)*src++)))
		++dst;
	return dst;
	}
