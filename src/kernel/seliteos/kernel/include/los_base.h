/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Basic definitions
 * Author: Huawei LiteOS Team
 * Create: 2022-06-15
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
 * @defgroup kernel Kernel
 * @defgroup los_base Basic definitions
 * @ingroup kernel
 */

#ifndef LOS_BASE_H
#define LOS_BASE_H

#if defined(LOSCFG_BOX)
#include "syscalls.h"
#endif
#include "arch/cpu.h"
#include "los_builddef.h"
#include "los_typedef.h"
#include "los_printf.h"
#include "los_errno.h"
#include "los_io.h"
#ifndef CONFIG_NON_TEE_SUPPORT
#include "securec.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup los_base
 * Global variable represents the system's mode.
 */
extern UINT32  g_sysMode;

/**
 * @ingroup los_base
 * Define the flag to check whether the system is running in the kernel space currently.
 */
#define LOS_IN_KERNEL                               ((g_sysMode & LOS_MSTATUS_MPP) > 0)

/**
 * @ingroup los_base
 * Define the timeout interval as LOS_NO_WAIT.
 */
#define LOS_NO_WAIT                                 0

/**
 * @ingroup los_base
 * Define the timeout interval as LOS_WAIT_FOREVER.
 */
#define LOS_WAIT_FOREVER                            0xFFFFFFFFU

/**
 * @ingroup los_base
 * @brief Align the value (addr) by some bytes (boundary).
 *
 * @par Description:
 * Align the beginning of the object with the base address, with boundary bytes being
 * the smallest unit of alignment. The ALIGN function is same with #LOS_Align function.
 *
 * @attention
 * <ul>
 * <li>the value of boundary usually is 4,8,16,32.</li>
 * <li>users should avoid overflows due to alignment.</li>
 * </ul>
 *
 * @param addr     [IN] Type #UINTPTR The address you want to align.
 * @param boundary [IN] Type #UINT32 The alignment size.
 *
 * @retval #UINTPTR The address which have been aligned.
 * @par Dependency:
 * <ul><li>los_base.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_Align | TRUNCATE
 */
#ifndef ALIGN
#define ALIGN(addr, boundary)                       LOS_Align((addr), (boundary))
#endif

/**
 * @ingroup los_base
 * @brief Align the value (addr) by some bytes (size).
 *
 * @par Description:
 * Align the tail of the object with the base address, with size bytes being the
 * smallest unit of alignment.
 *
 * @attention
 * <ul>
 * <li>the value of size usually is 4,8,16,32.</li>
 * <li>users should avoid overflows due to alignment.</li>
 * </ul>
 *
 * @param addr     [IN] Type #UINTPTR The address you want to align.
 * @param size     [IN] Type #UINT32  The alignment size.
 *
 * @retval #UINTPTR The address which have been aligned.
 * @par Dependency:
 * <ul><li>los_base.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_Align | ALIGN
 */
#define TRUNCATE(addr, size)                        ((UINTPTR)(addr) & ~((size) - 1U))

/**
 * Read a UINT8 value from addr and stroed in value.
 */
#define READ_UINT8(value, addr)                     { (value) = READ8_MB(addr); }

/**
 * Read a UINT16 value from addr and stroed in addr.
 */
#define READ_UINT16(value, addr)                    { (value) = READ16_MB(addr); }

/**
 * Read a UINT32 value from addr and stroed in value.
 */
#define READ_UINT32(value, addr)                    { (value) = READ32_MB(addr); }

/**
 * Read a UINT64 value from addr and stroed in value.
 */
#define READ_UINT64(value, addr)                    { (value) = READ64_MB(addr); }

/**
 * Write a UINT8 value to addr.
 */
#define WRITE_UINT8(value, addr)                    WRITE8_MB((addr), (value))

/**
 * Write a UINT16 value to addr.
 */
#define WRITE_UINT16(value, addr)                   WRITE16_MB((addr), (value))

/**
 * Write a UINT32 value to addr.
 */
#define WRITE_UINT32(value, addr)                   WRITE32_MB((addr), (value))

/**
 * Write a UINT64 addr to addr.
 */
#define WRITE_UINT64(value, addr)                   WRITE64_MB((addr), (value))

/**
 * Get a UINT8 value from addr.
 */
#define GET_UINT8(addr)                             READ8_MB(addr)

/**
 * Get a UINT16 value from addr.
 */
#define GET_UINT16(addr)                            READ16_MB(addr)

/**
 * Get a UINT32 value from addr.
 */
#define GET_UINT32(addr)                            READ32_MB(addr)

/**
 * Get a UINT64 value from addr.
 */
#define GET_UINT64(addr)                            READ64_MB(addr)

/**
 * @ingroup los_base
 * @brief Align the address (addr) by some bytes (boundary) you specify.
 *
 * @par Description:
 * This API is used to align the address (addr) by some bytes (boundary) you specify.
 *
 * @attention
 * <ul>
 * <li>the value of boundary usually is 4,8,16,32.</li>
 * <li>users should avoid overflows due to alignment.</li>
 * </ul>
 *
 * @param address  [IN] Type #UINTPTR The address you want to align.
 * @param boundary [IN] Type #UINT32  The alignment size.
 *
 * @retval #UINTPTR The address which have been aligned.
 * @par Dependency:
 * <ul><li>los_base.h: the header file that contains the API declaration.</li></ul>
 * @see ALIGN | TRUNCATE
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE UINTPTR LOS_Align(UINTPTR address, UINT32 boundary)
{
    return (address + boundary - 1U) & ~((UINTPTR)(boundary - 1U));
}

#define los_chk_return_no_print(cond, ret, ...) \
    do {                                        \
        if (cond) {                             \
            return ret;                         \
        }                                       \
    } while (0)

#define los_chk_goto_no_print(cond, label, ...) \
    do {                                        \
        if (cond) {                             \
            goto label;                         \
        }                                       \
    } while (0)

#define los_chk_return(cond, ret, ...) \
    do {                               \
        if (cond) {                    \
            PRINT_ERR(__VA_ARGS__);    \
            return ret;                \
        }                              \
    } while (0)

#define los_chk_goto(cond, label, ...) \
    do {                               \
        if (cond) {                    \
            PRINT_ERR(__VA_ARGS__);    \
            goto label;                \
        }                              \
    } while (0)

#define  los_chk_goto_with_ret(ret, cond, label, err_ret, ...) do {           \
    if (cond) {                                                               \
        PRINT_ERR(__VA_ARGS__);                                               \
        ret = err_ret;                                                        \
        goto label;                                                           \
    }                                                                         \
} while (0)

#ifndef KERNEL_UUID
#define KERNEL_UUID                                        \
    {                                                      \
        0x4deff1e8, 0xd24f, 0x41e5,                        \
        {                                                  \
            0x88, 0xcb, 0x87, 0x32, 0xe4, 0x72, 0xef, 0xce \
        }                                                  \
    }
#endif


#define TESVEC_ENTRY __attribute__((tesvec_entry))
#define TESVEC_ALIGN __attribute__((aligned(4)))
#define SECURE_CODE __attribute__((section(".text.secure")))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOS_BASE_H */
