/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: LiteOS Shell Command Implementation Headfile
 * Author: Huawei LiteOS Team
 * Create: 2023-01-01
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

#ifndef _HWLITEOS_SHELL_SHCMDDEF_H
#define _HWLITEOS_SHELL_SHCMDDEF_H

#include "los_list.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Hook for user-defined debug function
 * Unify different module's func for registration
 */
typedef UINT32 (*CmdCallBackFunc)(UINT32 argc, const CHAR **argv);

typedef BOOL (*CmdVerifyTransID)(UINT32 transId);

/* All support cmd types */
typedef enum {
    CMD_TYPE_SHOW = 0,
    CMD_TYPE_STD = 1,
    CMD_TYPE_EX = 2,
    CMD_TYPE_BUTT
} CmdType;

typedef struct {
    CmdType cmdType;
    CHAR *cmdKey;
    UINT32 paraNum;
    CmdCallBackFunc cmdHook;
} CmdItem;

typedef struct {
    LOS_DL_LIST list;
    CmdItem *cmd;
} CmdItemNode;

/* info for shell module */
typedef struct {
    CmdItemNode cmdList;
    UINT32 listNum;
    UINT32 initMagicFlag;
    UINT32 muxLock;
    CmdVerifyTransID transIdHook;
} CmdModInfo;

typedef struct {
    UINT32 count;
    LOS_DL_LIST list;
    CHAR cmdString[0];
} CmdKeyLink;

#ifdef __cplusplus
}
#endif

#endif /* _HWLITEOS_SHELL_SHCMDDEF_H */
