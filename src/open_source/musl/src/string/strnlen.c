#include <string.h>

size_t __strnlen(const char *s, size_t n)
{
	const char *p = memchr(s, 0, n);
	return p ? p-s : n;
}
weak_alias(__strnlen, strnlen);
