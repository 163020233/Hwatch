/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description : Safety LiteOS build module implementation.
 * Author: Huawei LiteOS Team
 * Create : 2022-07-02
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

/**
 * @defgroup los_builddef
 * @ingroup kernel
 */

#ifndef _LOS_BUILDEF_TARGET_H
#define _LOS_BUILDEF_TARGET_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup los_builddef
 * Allow inline sections
 */
#ifndef LITE_OS_SEC_ALW_INLINE
#define LITE_OS_SEC_ALW_INLINE                         __attribute__((always_inline))
#endif

/**
 * @ingroup los_builddef
 * .Text section (Code section)
 */
#ifndef LITE_OS_SEC_TEXT
#define LITE_OS_SEC_TEXT                                 __attribute__((section(".kernel.text")))
#endif

/**
 * @ingroup los_builddef
 * .Text.init section
 */
#ifndef LITE_OS_SEC_TEXT_INIT
#define LITE_OS_SEC_TEXT_INIT                            __attribute__((section(".init.kernel.text")))
#endif

#ifndef LITE_OS_SEC_KEEP_TEXT_ITCM
#define LITE_OS_SEC_KEEP_TEXT_ITCM                       __attribute__((section(".itcm.kernel.text")))
#endif

/**
 * @ingroup los_builddef
 * .Data section
 */
#ifndef LITE_OS_SEC_DATA
#define LITE_OS_SEC_DATA                                  __attribute__((section(".kernel.data")))
#endif

/**
 * @ingroup los_builddef
 * Not initialized variable section
 */
#ifndef LITE_OS_SEC_BSS
#define LITE_OS_SEC_BSS                                  __attribute__((section(".kernel.bss")))
#endif

#ifndef LITE_OS_SEC_TEXT_USRSPACE
#define LITE_OS_SEC_TEXT_USRSPACE                        __attribute__((section(".usr.text")))
#endif

#ifndef LITE_OS_SEC_DATA_USRSPACE
#define LITE_OS_SEC_DATA_USRSPACE                        __attribute__((section(".usr.data")))
#endif

#ifndef LITE_OS_SEC_BSS_USRSPACE
#define LITE_OS_SEC_BSS_USRSPACE                         __attribute__((section(".usr.bss")))
#endif

#ifndef LITE_OS_SEC_RODATA_USRSPACE
#define LITE_OS_SEC_RODATA_USRSPACE                      __attribute__((section(".usr.ro.data")))
#endif

#ifndef LITE_OS_SEC_DRVBOX_TEXT
#define LITE_OS_SEC_DRVBOX_TEXT                          __attribute__((section(".drvbox.text")))
#endif

#ifndef LITE_OS_SEC_DRVBOX_DATA
#define LITE_OS_SEC_DRVBOX_DATA                          __attribute__((section(".drvbox.data")))
#endif

#ifndef LITE_OS_SEC_DRVBOX_DATA
#define LITE_OS_SEC_DRVBOX_DATA                          __attribute__((section(".drvbox.bss")))
#endif

#ifndef LITE_OS_SEC_WEAK
#define LITE_OS_SEC_WEAK                                 __attribute__((weak))
#endif

#ifndef LITE_OS_ATTR_ALIGN
#define LITE_OS_ATTR_ALIGN(x)  __attribute__((aligned(x)))
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_BUILDEF_TARGET_H */
