/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: Hwi Shell Implementation
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

#include "los_hwi_pri.h"
#ifdef LOSCFG_SHELL
#include "shcmd.h"
#endif /* LOSCFG_SHELL */

LITE_OS_SEC_TEXT_MINOR UINT32 OsShellCmdHwi(INT32 argc, const CHAR **argv)
{
    UINTPTR hwiNum;
    UINTPTR devId;
    CHAR *end = NULL;

    if (argc == 0) {
        PrintHwiInfo();
    } else if ((argc == 2) && (strcmp(argv[0], "status") == 0)) { // 2 argvs
        if (strcmp(argv[1], "all") == 0) {
            OsHwiStatusAllPrint();
        } else {
            hwiNum = strtoul(argv[1], &end, 10); // 10 decimalism
            if ((*end != '\0') || (hwiNum > UINT32_MAX)) {
                PRINTK("hwiNum %s invalid\n", argv[1]);
                return OS_ERROR;
            }
            OsHwiStatusPrint((UINT32)hwiNum, NULL);
        }
    } else if ((argc == 3) && (strcmp(argv[0], "status") == 0)) { // 3 argvs, include device ID
        hwiNum = strtoul(argv[1], &end, 10); // 10 decimalism
        if ((*end != '\0') || (hwiNum > UINT32_MAX)) {
            PRINTK("hwiNum %s invalid\n", argv[1]);
            return OS_ERROR;
        }
        devId = strtoul(argv[2], &end, 16); // 2 argv represents devID, 16 Hexadecimal
        if (*end != '\0') {
            PRINTK("devId %s invalid\n", argv[2]); // 2 argv represents devID
            return OS_ERROR;
        }
        OsHwiStatusPrint((UINT32)hwiNum, (VOID *)devId);
    } else {
        PRINTK("\nUsage:\n"
            "hwi\n"
            "hwi status all\n"
            "hwi status hwiNum [devId]\n");
        return OS_ERROR;
    }

    return 0;
}

#ifdef LOSCFG_SHELL
SHELLCMD_ENTRY(hwi_shellcmd, CMD_TYPE_EX, "hwi", 0, (CmdCallBackFunc)OsShellCmdHwi);
#endif /* LOSCFG_SHELL */