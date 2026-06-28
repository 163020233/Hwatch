/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Lowpower Shell Implementation
 * Author: Huawei LiteOS Team
 * Create: 2021-11-12
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
#include "los_base.h"

#define ADDR_READ_DEF_CNT   64
#define ADDR_READ_UINT_CNT  4
#define ADDR_VAL_MEN_CNT    8

LITE_OS_SEC_TEXT_MINOR UINT32 SystemMd(INT32 argc, const CHAR **argv)
{
    if (argc != 2 && argc != 3) { // 2 addr, 3 addr cnt
        PRINTK("Usage: md addr [cnt] - addr read\n");
        return OS_ERROR;
    }

    UINT32 addr = (UINT32)strtoul(argv[1], NULL, 0);

    UINT32 cnt = ADDR_READ_DEF_CNT;
    if (argc == 3) { cnt = (UINT32)strtoul(argv[2], NULL, 0); } // 3 addr cnt, 2 addr cnt val

    UINT8 j, c;
    for (UINT32 i = 0; i < cnt; i++) {
        if (i % ADDR_READ_UINT_CNT == 0) { PRINTK("\n0x%08x:  ", addr); }
        PRINTK("0x");

        UINT32 value = *((volatile UINT32 *)addr);
        for (j = 0; j < ADDR_VAL_MEN_CNT; j++) {
            c = (value >> ((ADDR_VAL_MEN_CNT - j - 1) * ADDR_READ_UINT_CNT)) & 0x0F; // 0x0F l 4bit
            c = (c >= 10) ? (c - 10) + 'A' : c + '0'; // 10 character range
            PRINTK("%c", c);
        }
        PRINTK(" ");

        addr += sizeof(UINT32);
    }

    return 0;
}

LITE_OS_SEC_TEXT_MINOR UINT32 SystemMw(INT32 argc, const CHAR **argv)
{
    if (argc != 3 && argc != 4) { // 3 cmd addr val, 4 cmd addr val cnt
        PRINTK("Usage: mw addr val [cnt] -addr write\n");
        return OS_ERROR;
    }

    UINT32 addr = (UINT32)strtoul(argv[1], NULL, 0);

    UINT32 val = (UINT32)strtoul(argv[2], NULL, 0); // 2 is write val
    UINT32 cnt = 1;
    if (argc == 4) { cnt = (UINT32)strtoul(argv[3], NULL, 0); } // 4 is write cnt   3 is param id

    while (cnt-- > 0) {
        *(volatile UINT32 *)addr = val;
        addr += sizeof(UINT32);
    }
    return 0;
}

#ifdef LOSCFG_SHELL
SHELLCMD_ENTRY(md_shellcmd, CMD_TYPE_STD, "md", 3, (CmdCallBackFunc)SystemMd);
SHELLCMD_ENTRY(mw_shellcmd, CMD_TYPE_STD, "mw", 2, (CmdCallBackFunc)SystemMw);
#endif
