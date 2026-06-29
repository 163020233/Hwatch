/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: SIGNAL ERROR
 * Author: Huawei LiteOS Team
 * Create: 2020-01-15
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

#include "errno.h"
#include "los_task.h"
#ifdef LOSCFG_BOX
#include "pprivate.h"
#endif

#ifndef LOSCFG_BOX
#ifndef LOSCFG_LIB_CONFIGURABLE
int g_errnoArray[LOSCFG_BASE_CORE_TSK_LIMIT + 1];
#else /* LOSCFG_LIB_CONFIGURABLE */
__attribute__((section(".libc.errno"))) int g_errnoArray0[LOSCFG_BASE_CORE_TSK_LIMIT + 1];
int *g_errnoArray = &g_errnoArray0[0];
#endif /* LOSCFG_LIB_CONFIGURABLE */
/* the specific errno get or set in interrupt service routine */
static int g_errnoIsr;

int *__errno_location(void)
{
    if (OS_INT_INACTIVE) {
        return &g_errnoArray[LOS_CurTaskIDGet()];
    } else {
        return &g_errnoIsr;
    }
}
#else /* LOSCFG_BOX */
LITE_OS_SEC_BSS STATIC INT32 g_errnoArraykernel[LOSCFG_BASE_CORE_TSK_LIMIT + 1];
LITE_OS_SEC_BSS STATIC INT32 g_errnoIsr;

int *__errno_location(void)
{
    UINT32 boxTaskId = LOS_CurTaskIDGet();
    _pthread_data *data = NULL;

    if (OS_INT_ACTIVE) {
        return &g_errnoIsr;
    }
    if (OS_IN_KER_STATE) {
        return &g_errnoArraykernel[boxTaskId];
    }

    data = pthread_get_self_data();
    if (data == NULL) {
        PRINT_ERR("Cur box hasn't inited posix.\n");
        while (1) {
            ;
        }
    }
    /* No need to differ isr and task for isloated kernel due to unicore */
    return &data->threadErrno;
}
#endif /* LOSCFG_BOX */

void set_errno(int errCode)
{
    /* errno can not be set to 0 as posix standard */
    if (errCode == 0) {
        return;
    }
    *__errno_location() = errCode;
}

int get_errno(void)
{
    return *__errno_location();
}

volatile int *__errno(void)
{
    return (volatile int *)(__errno_location());
}
