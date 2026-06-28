#include <string.h>

#if defined(LOSCFG_LMS_LIBC_HIGH_FREQ_FUNC_CHECK)
LITE_OS_ATTR_NO_SANITIZE char *__strcat(char *restrict dest, const char *restrict src)
#else
char *__strcat(char *restrict dest, const char *restrict src)
#endif
{
	strcpy(dest + strlen(dest), src);
	return dest;
}
weak_alias(__strcat, strcat);
