/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: ShellCmd Swtmr
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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
#ifdef LOSCFG_SHELL
#include "shcmd.h"
#endif
#include "los_swtmr_pri.h"

LITE_OS_SEC_TEXT_MINOR UINT32 OsShellCmdSwtmrInfoGet(INT32 argc, const UINT8 **argv)
{
    UINT32 timerId;
    CHAR *endPtr = NULL;

    if (argc > 1) {
        PRINTK("\nUsage: swtmr [ID]\n");
        return OS_ERROR;
    }

    if (argc == 0) {
        timerId = OS_ALL_SWTMR_MASK;
    } else {
        timerId = strtoul((CHAR *)argv[0], &endPtr, 0);
        if ((endPtr == NULL) || (*endPtr != 0) || (timerId > LOSCFG_BASE_CORE_SWTMR_LIMIT)) {
            PRINTK("\nswtmr ID can't access %s.\n", argv[0]);
            return OS_ERROR;
        }
    }

    return OsDbgSwtmrInfoGet(timerId);
}

#ifdef LOSCFG_SHELL
SHELLCMD_ENTRY(swtmr_shellcmd, CMD_TYPE_EX, "swtmr", 1, (CmdCallBackFunc)OsShellCmdSwtmrInfoGet);
#endif
