/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Definition of Mutex
 * Author: Huawei LiteOS Team
 * Create: 2022-06-18
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
 * @defgroup los_mux Mutex
 * @ingroup kernel
 */

#ifndef LOS_MUX_H
#define LOS_MUX_H

#include "los_base.h"
#include "los_list.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Mutex base object must be the same as the first three member names of LosMuxCB,
 * so that pthread_mutex_t can share the kernel mutex mechanism.
 */
typedef struct {
    LOS_DL_LIST muxList; /* Mutex linked list */
    VOID *owner;         /* The current thread that is locking a mutex */
    UINT16 muxCount;     /* Times of locking a mutex */
} MuxBaseCB;

/**
 * @ingroup los_mux
 * Mutex error code: The mutex is not usable.
 *
 * Value: 0x02001d01.
 *
 * Solution: Check whether the mutex ID and the mutex state are applicable for the current operation.
 */
#define LOS_ERRNO_MUX_INVALID               LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x01)

/**
 * @ingroup los_mux
 * Mutex error code: Null pointer.
 *
 * Value: 0x02001d02.
 *
 * Solution: Check whether the input parameter is usable.
 */
#define LOS_ERRNO_MUX_PTR_NULL              LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x02)

/**
 * @ingroup los_mux
 * Mutex error code: No mutex is available and the mutex request fails.
 *
 * Value: 0x02001d03.
 *
 * Solution: Increase the number of mutexes defined by LOSCFG_BASE_IPC_MUX_LIMIT.
 */
#define LOS_ERRNO_MUX_ALL_BUSY              LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x03)

/**
 * @ingroup los_mux
 * Mutex error code: The mutex fails to be locked in non-blocking mode because it is locked by another thread.
 *
 * Value: 0x02001d04.
 *
 * Solution: Lock the mutex after it is unlocked by the thread that owns it, or set a waiting time.
 */
#define LOS_ERRNO_MUX_UNAVAILABLE           LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x04)

/**
 * @ingroup los_mux
 * Mutex error code: The mutex is being locked during an interrupt.
 *
 * Value: 0x02001d05.
 *
 * Solution: Check whether the mutex is being locked during an interrupt.
 */
#define LOS_ERRNO_MUX_PEND_INTERR           LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x05)

/**
 * @ingroup los_mux
 * Mutex error code: A thread locks a mutex after waiting for the mutex to be unlocked by another thread
 * when the task scheduling is disabled.
 *
 * Value: 0x02001d06.
 *
 * Solution: Check whether the task scheduling is disabled, or set timeout to 0, which means that the
 * thread will not wait for the mutex to become available.
 */
#define LOS_ERRNO_MUX_PEND_IN_LOCK          LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x06)

/**
 * @ingroup los_mux
 * Mutex error code: The mutex locking times out.
 *
 * Value: 0x02001d07.
 *
 * Solution: Increase the waiting time or set the waiting time to LOS_WAIT_FOREVER (forever-blocking mode).
 */
#define LOS_ERRNO_MUX_TIMEOUT               LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x07)

/**
 * @ingroup los_mux
 * Mutex error code: The mutex to be deleted is being locked.
 *
 * Value: 0x02001d09.
 *
 * Solution: Delete the mutex after it is unlocked.
 */
#define LOS_ERRNO_MUX_PENDED                LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x09)

/**
 * @ingroup los_mux
 * Mutex error code: The mutex is posted by non-owner.
 *
 * Value: 0x02001d0d.
 *
 * Solution: Post the mutex in the owner task.
 */
#define LOS_ERRNO_MUX_NOT_OWNER_POST        LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x0D)

/**
 * @ingroup los_mux
 * The error code will comes out only if the LOS_MUX_TYPE_ERRORCHECK value is set, which represents
 * the owner attempts to acquire the lock more than one time.
 * Value: 0x02001d10.
 *
 * Solution: Check the value whether the LOS_MUX_TYPE_ERRORCHECK attribute is set or multi-times
 *           pending operation by owner.
 */
#define LOS_ERRNO_MUX_DEAD_LOCK             LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x10)

/**
 * @ingroup los_mux
 * Mutex error code: The setting mutex attribute is invalid.
 * Value: 0x02001d11.
 *
 * Solution: Check all the mutex attribute value whether it is in the valid range.
 */
#define LOS_ERRNO_MUX_INVAL_ATTR            LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x11)

/**
 * @ingroup los_mux
 * Mutex error code: The OsSIdModuleInit initialized process in OsMuxInit function is failed.
 * Value: 0x02001d12.
 *
 * Solution: Check the validity of passed-in type and sIdObjList arguments.
 */
#define LOS_ERRNO_MUX_INIT_SID              LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x12)

/**
 * @ingroup los_mux
 * Mutex error code: Fail to generate sIdObj.
 * Value: 0x02001d13.
 *
 * Solution: Check the sIdObj generation procedure.
 */
#define LOS_ERRNO_MUX_GEN_SID_OBJ           LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x13)

/**
 * @ingroup los_mux
 * Mutex error code: Fail to delete sIdObj.
 * Value: 0x02001d14.
 *
 * Solution: Check the sIdObj delete procedure.
 */
#define LOS_ERRNO_MUX_DEL_SID_OBJ           LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x14)

/**
 * @ingroup los_mux
 * Mutex error code: The suspended resource has been destroyed.
 * Value: 0x02001d15.
 *
 * Solution: Create and use another mutex.
 */
#define LOS_ERRNO_MUX_DESTROYED             LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x15)

/**
 * @ingroup los_mux
 * Mutex error code: The input reference is pointing to illegal address
 * Value: 0x02001d16.
 *
 * Solution: Check the input value reference to make sure it is legal to access for current status.
 */
#define LOS_ERRNO_MUX_INVAL_ADDR            LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x16)

/**
 * @ingroup los_mux
 * Mutex error code: The API is called during an interrupt, which is forbidden.
 * Value: 0x02001d17.
 *
 * Solution: Perform this operation after exiting from the interrupt.
 */
#define LOS_ERRNO_MUX_CALLED_INTERR         LOS_ERRNO_OS_ERROR(LOS_MOD_MUX, 0x17)

/**
 * @ingroup los_mux
 * @brief Create a mutex.
 *
 * @par Description:
 * This API is used to create a mutex. A mutex handle is assigned to sId when the mutex is created successfully.
 * Return #LOS_OK when creating is successful, return specific error code otherwise.
 * @attention
 * The total number of mutexes is pre-configured. If there are no available mutexes, the mutex creation will fail.
 *
 * @param sId      [IN] The sId, source Id, is used to identify mutex object globally,
 *                      which should be unique and not zero.
 *
 * @retval #LOS_ERRNO_MUX_ALL_BUSY           No available mutex.
 * @retval #LOS_ERRNO_MUX_INVAL_ATTR         The setting mutex attribute value is invalid.
 * @retval #LOS_ERRNO_SEC_INVALID_SID        The input sId is invalid.
 * @retval #LOS_ERRNO_SEC_PERM_DENIED        The sId operation is permission denied.
 * @retval #LOS_ERRNO_MUX_GEN_SID_OBJ        Failed to generate sIdObj.
 * @retval #LOS_ERRNO_MUX_CALLED_INTERR      LOS_MuxCreate is called in interrupt context.
 * @retval #LOS_OK                           The mutex is successfully created.
 * @par Dependency:
 * <ul><li>los_mux.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_MuxDelete
 */
extern UINT32 LOS_MuxCreate(UINT32 *sId);

/**
 * @ingroup los_mux
 * @brief Delete a mutex.
 *
 * @par Description:
 * This API is used to delete a specified mutex. Return LOS_OK if the mutex deletion is successfully,
 * otherwise return specific error code.
 * @attention
 * <ul>
 * <li>The specific mutex should be created firstly.</li>
 * <li>The mutex can be deleted successfully only when there is no other tasks pend on it.</li>
 * </ul>
 *
 * @param sId      [IN] The sId, source Id, is used to identify mutex object globally,
 *                      which should be unique and not zero.
 *
 * @retval #LOS_ERRNO_MUX_INVALID            Invalid handle or mutex in use.
 * @retval #LOS_ERRNO_MUX_PENDED             Tasks pended on this mutex.
 * @retval #LOS_ERRNO_SEC_INVALID_SID        The input sId is invalid.
 * @retval #LOS_ERRNO_SEC_PERM_DENIED        The sId operation is permission denied.
 * @retval #LOS_ERRNO_SEC_INVALID_SIDMODULE  The module is not initialized.
 * @retval #LOS_ERRNO_MUX_DEL_SID_OBJ        Failed to delete sIdObj
 * @retval #LOS_ERRNO_MUX_CALLED_INTERR      LOS_MuxDelete is called in interrupt context.
 * @retval #LOS_OK                           The mutex is successfully deleted.
 * @par Dependency:
 * <ul><li>los_mux.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_MuxCreate
 */
extern UINT32 LOS_MuxDelete(UINT32 sId);

/**
 * @ingroup los_mux
 * @brief Wait to lock a mutex.
 *
 * @par Description:
 * This API is used to wait for a specified period of time to lock a mutex.
 * @attention
 * <ul>
 * <li>The specific mutex should be created firstly.</li>
 * <li>The function fails if the mutex that is waited on is already locked by another thread when the task scheduling
 * is disabled.</li>
 * <li>Do not wait on a mutex during an interrupt.</li>
 * <li>The priority inheritance protocol is supported. If a higher-priority thread is waiting on a mutex, it changes
 * the priority of the thread that owns the mutex to avoid priority inversion.</li>
 * <li>A recursive mutex can be locked more than once by the same thread.</li>
 * <li>Do not recommend to use this API in software timer callback. </li>
 * </ul>
 *
 * @param sId          [IN] The sId, source Id, is used to identify mutex object globally,
 *                      which should be unique and not zero.
 * @param timeout      [IN] Waiting time. The value range is [0, LOS_WAIT_FOREVER](unit: Tick).
 *
 * @retval #LOS_ERRNO_MUX_INVALID            The mutex state (for example, the mutex does not exist or is not in use)
 *                                           is not applicable for the current operation.
 * @retval #LOS_ERRNO_MUX_UNAVAILABLE        The mutex fails to be locked because it is locked by another thread and
 * a period of time is not set for waiting for the mutex to become available.
 * @retval #LOS_ERRNO_MUX_PEND_INTERR        The mutex is being locked during an interrupt.
 * @retval #LOS_ERRNO_MUX_PEND_IN_LOCK       The mutex is waited on when the task scheduling is disabled.
 * @retval #LOS_ERRNO_MUX_TIMEOUT            The mutex waiting times out.
 * @retval #LOS_ERRNO_SEC_INVALID_SID        The input sId is invalid.
 * @retval #LOS_ERRNO_SEC_PERM_DENIED        The sId operation is permission denied.
 * @retval #LOS_ERRNO_SEC_INVALID_SIDMODULE  The module is not initialized.
 * @retval #LOS_ERRNO_MUX_DEAD_LOCK          Using CheckError attribute to check recursive pending operation.
 * @retval #LOS_OK                           The mutex is successfully locked.
 * @par Dependency:
 * <ul><li>los_mux.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_MuxCreate | LOS_MuxPost
 */
extern UINT32 LOS_MuxPend(UINT32 sId, UINT32 timeout);

/**
 * @ingroup los_mux
 * @brief Release a mutex.
 *
 * @par Description:
 * This API is used to release a specified mutex.
 * @attention
 * <ul>
 * <li>The specific mutex should be created firstly.</li>
 * <li>Do not release a mutex during an interrupt.</li>
 * <li>If a recursive mutex is locked for many times, it must be unlocked for the same times to be released.</li>
 * </ul>
 *
 * @param sId      [IN] The sId, source Id, is used to identify mutex object globally,
 *                      which should be unique and not zero.
 *
 * @retval #LOS_ERRNO_MUX_INVALID            The mutex state (for example, the mutex does not exist or is not in use
 * or owned by other thread) is not applicable for the current operation.
 * @retval #LOS_ERRNO_MUX_PEND_INTERR        The mutex is being released during an interrupt.
 * @retval #LOS_ERRNO_SEC_INVALID_SID        The input sId is invalid.
 * @retval #LOS_ERRNO_SEC_PERM_DENIED        The sId operation is permission denied.
 * @retval #LOS_ERRNO_SEC_INVALID_SIDMODULE  The module is not initialized.
 * @retval #LOS_OK                           The mutex is successfully released.
 * @par Dependency:
 * <ul><li>los_mux.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_MuxCreate | LOS_MuxPend
 */
extern UINT32 LOS_MuxPost(UINT32 sId);

/**
 * @ingroup los_mux
 * @brief Judge whether current task holds the mutex.
 *
 * @par Description:
 * This API is used to judge whether current task holds the mutex.
 * @attention
 * <ul>
 * <li>The specific mutex should be created firstly.</li>
 * </ul>
 *
 * @param sId      [IN]  Type  #UINT32 The sId, source Id, is used to identify mutex object globally,
 *                       which should be unique and not zero.
 *
 * @retval #LOS_ERRNO_SEC_INVALID_SID        The input sId is invalid.
 * @retval #LOS_ERRNO_SEC_PERM_DENIED        The sId operation is permission denied.
 * @retval #LOS_ERRNO_SEC_INVALID_SIDMODULE  The module is not initialized.
 * @retval #LOS_ERRNO_MUX_CALLED_INTERR      LOS_MuxHeld is called in interrupt context.
 * @retval #TRUE                             The mutex is held by current task.
 * @retval #FALSE                            The mutex is not held by current task.
 * @par Dependency:
 * <ul><li>los_mux.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_MuxCreate
 */
extern UINT32 LOS_MuxHeld(UINT32 sId);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOS_MUX_H */
