#include <time.h>

#ifdef LOSCFG_BOX

#include "los_box_pri.h"
#include "los_perbox_var_pri.h"
#include "los_task_pri.h"

static char *get_asctime_ptr_addr(void)
{
	PerboxData *boxPriData = (PerboxData *)OsPerboxDataAddrGet(LOS_CurTaskIDGet());
    if (boxPriData == NULL || boxPriData->timeData == NULL) {
		return NULL;
	}

	return boxPriData->timeData->asctimeBuf;
}

char *asctime(const struct tm *tm)
{
	char *buf = NULL;
	if (OS_IN_KER_STATE) {
		static char kerBuf[26];
		buf = kerBuf;
	} else {
		char *usrBuf = get_asctime_ptr_addr();
		buf = usrBuf;
	}
	if (buf == NULL) {
		return NULL;
	}

	return __asctime_r(tm, buf);
}
#else
char *asctime(const struct tm *tm)
{
	static char buf[26];
	return __asctime_r(tm, buf);
}
#endif
