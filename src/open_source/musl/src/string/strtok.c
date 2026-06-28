#include <string.h>

#ifdef LOSCFG_BOX
#include "los_box_pri.h"
#include "los_perbox_var_pri.h"
#include "los_task_pri.h"

static char **get_strtok_ptr_addr(void)
{
	PerboxData *boxPriData = (PerboxData *)OsPerboxDataAddrGet(LOS_CurTaskIDGet());
	if (boxPriData == NULL) {
		return NULL;
	}

	return &boxPriData->strtokPtr;
}
#endif

char *strtok(char *restrict s, const char *restrict sep)
{
#ifdef LOSCFG_BOX
	if (!OS_IN_KER_STATE) {
		char **p = get_strtok_ptr_addr();
		if (p == NULL) return NULL;

		if (!s && !(s = *p)) return NULL;
		s += strspn(s, sep);
		if (!*s) return *p = 0;
		*p = s + strcspn(s, sep);
		if (**p) **p++ = 0;
		else *p = 0;
		return s;
	}
#endif
	static char *p;
	if (!s && !(s = p)) return NULL;
	s += strspn(s, sep);
	if (!*s) return p = 0;
	p = s + strcspn(s, sep);
	if (*p) *p++ = 0;
	else p = 0;
	return s;
}
