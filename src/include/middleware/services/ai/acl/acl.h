/**
* @file acl.h
*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2025. All rights reserved.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef INC_EXTERNAL_ACL_ACL_H_
#define INC_EXTERNAL_ACL_ACL_H_

#include "acl_rt.h"
#include "acl_mdl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup AscendCL
 * @brief acl initialize
 *
 * @par Restriction
 * This interface can be called multiple times in a process, the reference count will
 * increase by 1 each time when aclInit is called multiple times.
 * The aclFinalizeReference interface should be called the same number of times as aclInit,
 * or you can call aclFinalize to release all resources regardless of the reference count and reset it to 0.
 *
 * @param configPath [IN]    the config path,it can be NULL
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclInit(const char *configPath);

/**
 * @ingroup AscendCL
 * @brief acl finalize
 *
 * @par Restriction
 * This interface should be called before the process exits.
 * After calling aclFinalize, all allocated resources are released,
 * the internal reference count is reset to 0, and ACL services can no longer be used.
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclFinalize(void);

/**
 * @ingroup AscendCL
 * @brief get recent error message
 *
 * @retval null for failed
 * @retval OtherValues success
*/
ACL_FUNC_VISIBILITY const char *aclGetRecentErrMsg(void);

#ifdef __cplusplus
}
#endif

#endif // INC_EXTERNAL_ACL_ACL_H_
