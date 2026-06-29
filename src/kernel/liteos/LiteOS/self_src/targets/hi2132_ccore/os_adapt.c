/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: main process
 * Author: Huawei LiteOS Team
 * Create: 2020-08-10
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#include <stdint.h>
#include "los_typedef.h"
#include "los_printf.h"
#include "los_task.h"
#include "los_init_pri.h"
#include "los_task_pri.h"
#include "los_init.h"
#include "hal_hwi.h"

/* modify default hwi triggle mode */
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

VOID LOS_PrepareMainTask(VOID)
{
    OsSetMainTask();
    OsCurrTaskSet(OsGetMainTask());
}
