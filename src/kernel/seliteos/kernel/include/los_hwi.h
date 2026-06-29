/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Hwi HeadFile
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
 * @defgroup los_hwi Hardware interrupt
 * @ingroup kernel
 */
#ifndef LOS_HWI_H
#define LOS_HWI_H

#include "los_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup los_hwi
 * @brief Disable all interrupts.
 *
 * @par Description:
 * This API is used to disable all interrupts.
 * @attention
 * None.
 *
 * @param None.
 *
 * @retval #UINT32 Interrupt status before all interrupts are disabled.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_IntRestore
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE UINT32 LOS_IntLock(VOID)
{
    return LOS_OK;
}

/**
 * @ingroup los_hwi
 * @brief Enable all interrupts.
 *
 * @par Description:
 * This API is used to enable all interrupts.
 * @attention
 * None.
 *
 * @param None.
 *
 * @retval #UINT32 Interrupt status after all interrupts are enabled.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_IntLock
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE UINT32 LOS_IntUnLock(VOID)
{
    return LOS_OK;
}

/**
 * @ingroup los_hwi
 * @brief Restore interrupts.
 *
 * @par Description:
 * This API is used to restore the interrupt status obtained before all interrupts are disabled by #LOS_IntLock.
 * @attention
 * This API can be called only after #LOS_IntLock, and the input parameter value should be
 * the value returned by #LOS_IntLock.
 *
 * @param intSave [IN] Type #UINT32. Interrupt status before all interrupts are disabled.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_IntLock
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_IntRestore(UINT32 intSave)
{
    (void)intSave;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOS_HWI_H */
