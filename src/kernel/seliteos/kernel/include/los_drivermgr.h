/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Driver box management module head file
 * Author: Huawei LiteOS Team
 * Create: 2020-07-31
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
 * @defgroup los_drivermgr Driver Isolation
 * @ingroup kernel
 */

#ifndef _LOS_DRVMGR_H
#define _LOS_DRVMGR_H

#include "syscalls.h"
#include "los_typedef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @ingroup los_drivermgr
 * Define the limitation of the max driver Id values.
 */
#define MAX_DRV_ID                32U

/**
 * @ingroup los_drivermgr
 * Return value of driver manager switching request: no need to do the switching operation
 * in current system status.
 *
 * Value: 1
 *
 * Solution: Check the validation of input sId.
 */
#define DRV_BOX_INVALID_SWITCH    1U

/**
 * @ingroup los_drivermgr
 * Return value of driver manager switching request: permission check failed.
 *
 * Value: 2
 *
 * Solution: Check the validation of input sId.
 */
#define DRV_BOX_SWITCH_FAIL       2U

/**
 * @ingroup los_drivermgr
 * @brief Switch region from usr box to driver box.
 *
 * @par Description:
 * This API is used to switch executing region from usr box to driver box, and change
 * pmp/mpu configuration respectivly.
 *
 * @attention
 * This api can only be invoked in common usr lib.
 *
 * @param    drvId    used to check the invoking permission of the driver.
 *
 * @retval   #DRV_BOX_INVALID_SWITCH  No need to do the switching process due to the limitation.
 * @retval   #DRV_BOX_SWITCH_FAIL     Permission rejected.
 * @retval   #LOS_OK                  Box region switching success.
 * @par Dependency: \
 * <ul><li>los_drivermgr.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_DrvmgrSwitch2Usrbox
 * @since SE LiteOS 1.0.0
 */
extern UINT32 LOS_DrvmgrSwitch2Drvbox(UINT32 drvId);

/**
 * @ingroup los_drivermgr
 * @brief Switch region from driver box to usr box
 *
 * @par Description:
 * This API is used to switch box running region from driver box to usr box, and change
 * pmp/mpu configuration respectivly.
 *
 * @attention
 * This api can only be invoked in common usr lib space.
 *
 * @param    VOID
 *
 * @retval   VOID
 * @par Dependency: \
 * <ul><li>los_drivermgr.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_DrvmgrSwitch2Drvbox
 * @since SE LiteOS 1.0.0
 */
extern VOID LOS_DrvmgrSwitch2Usrbox(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
