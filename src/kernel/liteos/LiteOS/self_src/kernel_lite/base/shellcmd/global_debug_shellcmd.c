/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: XBUG
 * Author: Huawei LiteOS Team
 * Create: 2021-11-08
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
#include "shell.h"
#endif
#include "los_swtmr_pri.h"
#include "los_hwi_pri.h"
#include "los_sem_pri.h"
#include "los_task_pri.h"
#include "los_sem_debug_pri.h"

LITE_OS_SEC_TEXT_MINOR STATIC VOID OsShellCmdGlobalInfo(VOID)
{
    PRINTK("=====================XUBG=====================\n");
    PRINTK("\r\n=====================TASK INFO=====================\n");
    (void)OsDbgTskInfoGet(OS_ALL_TASK_MASK);
#ifdef LOSCFG_DEBUG_HWI
    PRINTK("\r\n=====================HWI INFO=====================\n");
    PrintHwiInfo();
#endif
    PRINTK("\r\n=====================STACK INFO=====================\n");
    OsExcStackInfo();
#ifdef LOSCFG_MEM_LEAKCHECK
    PRINTK("\r\n=====================MEM INFO=====================\n");
    OsMemUsedNodeShow(m_aucSysMem1);
#endif
#ifdef LOSCFG_DEBUG_SWTMR
    PRINTK("\r\n=====================SWTMR INFO=====================\n");
    OsDbgSwtmrInfoGet(OS_ALL_SWTMR_MASK);
#endif
}

#ifdef LOSCFG_SHELL
SHELLCMD_ENTRY(globalinfo_shellcmd, CMD_TYPE_EX, "xbug", 1, (CmdCallBackFunc)OsShellCmdGlobalInfo);
#endif /* LOSCFG_SHELL */