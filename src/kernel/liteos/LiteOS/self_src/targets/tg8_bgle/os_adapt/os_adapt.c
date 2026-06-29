/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: os platform config files
 */

#include <stdint.h>
#include "los_typedef.h"
#include "los_printf.h"
#include "los_task.h"
#include "los_init_pri.h"
#include "los_task_pri.h"
#include "los_init.h"

#define APP_INIT_WDT_KICK_TIME_MS 20000
extern UINT32 uapi_watchdog_kick(VOID);

/* need change hwi triggle mode */
LITE_OS_SEC_TEXT_INIT UINT32 OsAdaptInit(VOID)
{
    uint8_t clicintattr = 0;
    /* clicintattr[2:1] = 1 edge-triggered;
       clicintattr[2:1] = 0 level-triggered;
       clicintattr[7:6] = 3 M-mode */
    clicintattr |= ((0 << 1) | (3 << 6));
    for (uint8_t index = 0; index < LOSCFG_PLATFORM_HWI_LIMIT; index++) {
        HalIrqSetClicIntAttr(index, clicintattr);
    }
    return LOS_OK;
}
LOS_SYS_INIT(OsAdaptInit, SYS_INIT_LEVEL_APP, SYS_INIT_SYNC_1);