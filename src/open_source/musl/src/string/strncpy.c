#include <string.h>

#if defined(LOSCFG_LMS_LIBC_HIGH_FREQ_FUNC_CHECK)
LITE_OS_ATTR_NO_SANITIZE char *__strncpy(char *restrict d, const char *restrict s, size_t n)
#else
char *__strncpy(char *restrict d, const char *restrict s, size_t n)
#endif
{
	__stpncpy(d, s, n);
	return d;
}
weak_alias(__strncpy, strncpy);
