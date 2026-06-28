/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: CMSIS Interface
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

#ifndef _CMSIS_OS_H
#define _CMSIS_OS_H

#ifdef LOSCFG_COMPAT_CMSIS_VER_1
#include "cmsis_os1.h"
#endif

#ifdef LOSCFG_COMPAT_CMSIS_VER_2
#include "cmsis_os2.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_COMPAT_CMSIS_VER_2
/**
 * @ingroup cmsis
 * @brief Obtain system time in milliseconds.
 *
 * @par Description:
 * This API is used to Obtain system time in milliseconds(only for LiteOS).
 * @attention
 * None.
 *
 * @param  None.
 *
 * @retval   #UINT64 system time in milliseconds.
 * @par Dependency:
 * <ul>
 * <li>cmsis_os.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
uint64_t osKernelGetTick2ms(void);

/**
 * @ingroup cmsis
 * @brief Convert milliseconds to Ticks.
 *
 * @par Description:
 * This API is used to convert milliseconds to ticks(only for LiteOS).
 * @attention
 * None.
 *
 * @param  millisec  [IN] The milliseconds need to be converted to Ticks.
 *
 * @retval #UINT64 Number of Ticks obtained through the conversion.
 * @par Dependency:
 * <ul>
 * <li>cmsis_os.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
uint64_t osMs2Tick(uint64_t ms);

void *osThreadGetArgument(void);
#ifdef LOSCFG_BASE_CORE_SWTMR_ALIGN
typedef enum  {
    /* The timer is not allowed to wake up the RTOS. */
    osTimerRousesIgnore = 0,
    /* The timer is allowed to wake up the RTOS. */
    osTimerRousesAllow  = 1
} osTimerRouses_t;

typedef enum  {
    /* The timer ignores alignment. */
    osTimerAlignIgnore = 0,
    /* The timer allows alignment. */
    osTimerAlignAllow  = 1
} osTimerAlign_t;

osTimerId_t osTimerExtNew(osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr,
                          osTimerRouses_t rouses, osTimerAlign_t sensitive);
#endif /* LOSCFG_BASE_CORE_SWTMR_ALIGN */

typedef enum {
    CMSIS_THREAD_POLICY_DEFAULT      = 0, // Rt/RR/FIFO.
    CMSIS_THREAD_POLICY_RT           = 1, // Rt/RR/FIFO.
    CMSIS_THREAD_POLICY_CFS          = 2, // CFS.
} cmsis_thread_policy;
osThreadId_t osThreadNewExt(osThreadFunc_t func, void *argument, const osThreadAttr_t *attr,
                            cmsis_thread_policy policy);
osStatus_t osThreadSetPolicy(osThreadId_t thread_id, cmsis_thread_policy policy);
#endif /* LOSCFG_COMPAT_CMSIS_VER_2 */

#ifdef LOSCFG_COMPAT_CMSIS_VER_1
/**
 * @ingroup cmsis
 * @brief Clear Mail.
 *
 * @par Description:
 * This API is used to clear mail(only for LiteOS).
 * @attention
 * None.
 *
 * @param  osMailClear  [IN] mail queue ID obtained with osMailCreate.
 *
 * @retval #osStatus status code that indicates the execution status of the function.
 * @par Dependency:
 * <ul>
 * <li>cmsis_os.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
osStatus osMailClear(osMailQId queue_id);

/**
 * @ingroup cmsis
 * @brief Delete Mail.
 *
 * @par Description:
 * This API is used to Delete mail(only for LiteOS).
 * @attention
 * None.
 *
 * @param  osMailDelete  [IN] mail queue ID obtained with osMailCreate.
 *
 * @retval #osStatus status code that indicates the execution status of the function.
 * @par Dependency:
 * <ul>
 * <li>cmsis_os.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
osStatus osMailDelete(osMailQId queue_id);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _CMSIS_OS_H */
