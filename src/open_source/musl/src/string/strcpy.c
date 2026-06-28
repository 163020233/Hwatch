#include <string.h>

#if defined(LOSCFG_LMS_LIBC_HIGH_FREQ_FUNC_CHECK)
LITE_OS_ATTR_NO_SANITIZE char *__strcpy(char *restrict dest, const char *restrict src)
#else
char *__strcpy(char *restrict dest, const char *restrict src)
#endif
{
	__stpcpy(dest, src);
	return dest;
}
weak_alias(__strcpy, strcpy);
