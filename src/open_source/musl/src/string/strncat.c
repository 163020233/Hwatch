#include <string.h>

#if defined(LOSCFG_LMS_LIBC_HIGH_FREQ_FUNC_CHECK)
LITE_OS_ATTR_NO_SANITIZE char *__strncat(char *restrict d, const char *restrict s, size_t n)
#else
char *__strncat(char *restrict d, const char *restrict s, size_t n)
#endif
{
	char *a = d;
	d += strlen(d);
	while (n && *s) n--, *d++ = *s++;
	*d++ = 0;
	return a;
}
weak_alias(__strncat, strncat);
