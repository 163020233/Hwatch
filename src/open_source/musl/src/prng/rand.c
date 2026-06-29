#include <stdlib.h>
#include <stdint.h>

#ifdef LOSCFG_BOX
#include "los_box_pri.h"
#include "los_perbox_var_pri.h"
#include "los_task_pri.h"
static uint64_t seed;
static uint64_t *get_seed_pri_var_addr(void)
{
    PerboxData *boxPriData = (PerboxData *)OsPerboxDataAddrGet(LOS_CurTaskIDGet());
    PerboxPrngSeed *array = NULL;
    if (boxPriData == NULL) {
        return NULL;
    }
    array = boxPriData->prngSeed;
    if (array == NULL) {
        return NULL;
    }
    return &array->randSeed;
}

void srand(unsigned s)
{
	uint64_t *randSeed = NULL;
	if (OS_IN_KER_STATE) {
		randSeed = &seed;
	} else {
		uint64_t *tmpRandSeed = get_seed_pri_var_addr();
		randSeed = tmpRandSeed;
	}
	if (randSeed == NULL) {
		return;
	}
	*randSeed = s - 1;
}

int rand(void)
{
	uint64_t *randSeed = NULL;
	if (OS_IN_KER_STATE) {
		randSeed = &seed;
	} else {
		uint64_t *tmpRandSeed = get_seed_pri_var_addr();
		randSeed = tmpRandSeed;
	}
	if (randSeed == NULL) {
		return 0;
	}
	*randSeed = 6364136223846793005ULL * (*randSeed) + 1; /* 6364136223846793005ULL: randorm multiplier */
	return *randSeed >> 33;                               /* 33: shift bit number */
}

#else
static uint64_t seed;

void srand(unsigned s)
{
	seed = s-1;
}

int rand(void)
{
	seed = 6364136223846793005ULL*seed + 1;
	return seed>>33;
}
#endif
