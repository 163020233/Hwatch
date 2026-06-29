/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Exception internal HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2020-06-24
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

#ifndef _LOS_EXC_PRI_H
#define _LOS_EXC_PRI_H

#include "los_exc.h"
#include "los_printf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_SHELL_EXCINFO_DUMP
#define EXCINFO_PRINTK(fmt, ...)    do {                       \
            PRINTK(fmt, ##__VA_ARGS__);                        \
            WriteExcInfoToBuf(fmt, ##__VA_ARGS__);             \
        } while (0)

#define EXCINFO_PRINT_ERR(fmt, ...)    do {                    \
            PRINT_ERR(fmt, ##__VA_ARGS__);                     \
            WriteExcInfoToBuf(fmt, ##__VA_ARGS__);             \
        } while (0)

#define EXCINFO_PANIC(fmt, ...)    do {                        \
            WriteExcInfoToBuf(fmt, ##__VA_ARGS__);             \
            LOS_PANIC(fmt, ##__VA_ARGS__);                     \
        } while (0)
#else
#define EXCINFO_PRINTK(fmt, ...)    do {                       \
            PRINTK(fmt, ##__VA_ARGS__);                        \
        } while (0)

#define EXCINFO_PRINT_ERR(fmt, ...)    do {                    \
            PRINT_ERR(fmt, ##__VA_ARGS__);                     \
        } while (0)

#define EXCINFO_PANIC(fmt, ...)    do {                        \
            LOS_PANIC(fmt, ##__VA_ARGS__);                     \
        } while (0)
#endif

#ifdef LOSCFG_SHELL_EXCINFO_DUMP
extern VOID OsSetExcInfoRW(LogReadWriteFunc func);
extern LogReadWriteFunc OsGetExcInfoRW(VOID);
extern VOID OsSetExcInfoBuf(CHAR *buf);
extern CHAR *OsGetExcInfoBuf(VOID);
extern VOID OsSetExcInfoOffset(UINT32 offset);
extern UINT32 OsGetExcInfoOffset(VOID);
extern VOID OsSetExcInfoDumpAddr(UINTPTR addr);
extern UINTPTR OsGetExcInfoDumpAddr(VOID);
extern VOID OsSetExcInfoLen(UINT32 len);
extern UINT32 OsGetExcInfoLen(VOID);
extern VOID WriteExcBufVa(const CHAR *format, va_list arglist);
extern VOID WriteExcInfoToBuf(const CHAR *format, ...);
#endif

#ifdef LOSCFG_EXC_INTERACTION
extern UINT32 OsCheckExcInteractionTask(const TSK_INIT_PARAM_S *initParam);
extern VOID OsKeepExcInteractionTask(VOID);
#endif

extern VOID OsSmcHandler(const ExcContext *frame);
extern VOID OsSmcCall(UINT64 arg0, UINT64 arg1, UINT64 arg2, UINT64 arg3);

extern VOID LOS_TaskBackTrace_Hook(UINT32 taskID);
extern VOID LOS_BackTrace_Hook(VOID);

extern UINT32 HalIrqMaskGet(UINT32 hwiNum, UINT8 *isMask);
extern UINT32 HalIrqPendingGet(UINT32 hwiNum, UINT8 *isPending);
extern UINT32 OsIrqNestingCntGet(VOID);

#ifdef LOSCFG_KENREL_BULITIN_ROM
typedef struct {
    VOID (*osExcTrapHandle)(unsigned int irq_num);
} OsExcOps;
#endif

typedef struct {
    VOID (*osBackTrace)(VOID);
    VOID (*osTaskBackTrace)(const VOID *stackPointer);
} OsBackTraceOps;

typedef struct {
    UINT32       hwiNum;
    UINT32       pending;
    UINT32       enable;
    UINT32       nestingCount;
    UINT32       mcause;
    UINT32       mepc;
} HwiLoseInfo;

extern size_t g_excCount[LOSCFG_KERNEL_CORE_NUM];
extern size_t ExcActive(VOID);

#ifdef LOSCFG_BACKTRACE_SP
typedef struct {
    UINT32 text_begin;
    UINT32 text_end;
} LosTextAddrInfo;

/* 数组完整类型声明，dfx提供若定义, 用户必须覆盖为强定义 */
extern LosTextAddrInfo g_textAddrInfo[];
/* 数组大小变量声明，dfx提供弱定义，用户必须覆盖为强定义 */
extern INT32 g_textAddrInfoSize;
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_EXC_H */
