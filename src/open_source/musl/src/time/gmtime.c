#include "time_impl.h"
#include <errno.h>

#ifdef LOSCFG_BOX
#include "los_box_pri.h"
#include "los_perbox_var_pri.h"
#include "los_task_pri.h"

static struct tm *get_gmtm_ptr_addr(void)
{
	PerboxData *boxPriData = (PerboxData *)OsPerboxDataAddrGet(LOS_CurTaskIDGet());
    if (boxPriData == NULL || boxPriData->timeData == NULL) {
		return NULL;
	}

	return &boxPriData->timeData->gmtm;
}

struct tm *gmtime(const time_t *t)
{
	static struct tm kernel_tm;
	struct tm *tm;
	if (OS_IN_KER_STATE) {
		tm = &kernel_tm;
	} else {
		tm = get_gmtm_ptr_addr();
	}
	if (tm == NULL) {
		return NULL;
	}
	return __gmtime_r(t, tm);
}
#else
struct tm *gmtime(const time_t *t)
{
	static struct tm tm;
	return __gmtime_r(t, &tm);
}
#endif
