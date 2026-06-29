/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS Patch Management Module HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2024-09-25
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

#ifndef LOS_LP_H
#define LOS_LP_H

#include "los_sys.h"

/**
 * @ingroup los_lp
 * LP error code: The LP unit number is invalid. It is 0 or exceeds the maximum value.
 *
 * Value: 0x02004201.
 *
 * Solution: Check the validity of the LP unit number.
 */
#define LOS_ERRNO_LP_NO_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x01)

/**
 * @ingroup los_lp
 * LP error code: The tag header in the static patch file is incorrect.
 *
 * Value: 0x02004202.
 *
 * Solution: If the incremental flag of the static patch file is not 'SP', create an incremental patch file.
 */
#define LOS_ERRNO_LP_SP_FLAG_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x02)

/**
 * @ingroup los_lp
 * LP error code: The LP file is repeatedly loaded.
 *
 * Value: 0x02004203.
 *
 * Solution: Check whether the LP file is repeatedly loaded in the code and modify the code.
 */
#define LOS_ERRNO_LP_LOADED LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x03)

/**
 * @ingroup los_lp
 * LP error code: The code segment in the static patch file is inconsistent with that in the configuration file.
 *
 * Value: 0x02004204.
 *
 * Solution: Check whether the static patch code segment configuration in the makepatch.ini file is
 * consistent with that in the management group.
 */
#define LOS_ERRNO_LP_CODE_ADDR_NOT_SUIT LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x04)

/**
 * @ingroup los_lp
 * LP error code: The data segment in the static patch file is inconsistent with the shared or private
 * data segment in the configuration.
 *
 * Value: 0x02004205.
 *
 * Solution: Check whether the configurations of the static patch shared data segment and private data segment
 * in the makepatch.ini file are consistent with those in the management group.
 */
#define LOS_ERRNO_LP_DATA_ADDR_NOT_SUIT LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x05)

/**
 * @ingroup los_lp
 * LP error code: The LP unit is not loaded during the LP operation.
 *
 * Value: 0x02004206.
 *
 * Solution: Verify that the LP unit is loaded.
 */
#define LOS_ERRNO_LP_NOT_LOADED LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x06)

/**
 * @ingroup los_lp
 * LP error code: The LP unit has been activated.
 *
 * Value: 0x02004207.
 *
 * Solution: None.
 */
#define LOS_ERRNO_LP_IS_ACTIVE LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x07)

/**
 * @ingroup los_lp
 * LP error code: The LP unit has been deactivated.
 *
 * Value: 0x02004208.
 *
 * Solution: None.
 */
#define LOS_ERRNO_LP_IS_DEACTIVE LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x08)

/**
 * @ingroup los_lp
 * LP error code: The parameter pointer is null.
 *
 * Value: 0x02004209.
 *
 * Solution: Check whether the input pointer variable is null.
 */
#define LOS_ERRNO_LP_INPUT_IS_NULL LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x09)

/**
 * @ingroup los_lp
 * LP error code: The buffer length is insufficient or the buffer length is 0. Check whether
 * the input parameters are correct.
 *
 * Value: 0x0200420a.
 *
 * Solution: Check the length of the input buffer.
 */
#define LOS_ERRNO_LP_BUF_NOT_ENOUGH LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x0a)

/**
 * @ingroup los_lp
 * LP error code: Failed to apply for the memory. The memory needs to be dynamically applied
 * for during the LP operation.
 *
 * Value: 0x0200420b.
 *
 * Solution: Set the memory space to a large value.
 */
#define LOS_ERRNO_LP_MEM_NOT_ENOUGH LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x0b)

/**
 * @ingroup los_lp
 * LP error code: The start address of the static patch unit code segment is smaller than the start address of the
 * patch area code segment, or the static patch unit code segment exceeds the range of the patch area code segment.
 *
 * Value: 0x0200420c.
 *
 * Solution: Check the start address of the static patch unit code segment or increase the
 * size of the patch area code segment.
 */
#define LOS_ERRNO_LP_CODE_OVER_FLOW LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x0c)

/**
 * @ingroup los_lp
 * LP error code: The start address of the static patch unit data segment is smaller than the start address of the
 * patch area data segment, or the static patch unit data segment exceeds the range of the patch area data segment.
 *
 * Value: 0x0200420d.
 *
 * Solution: Check the start address of the static patch unit data segment or increase the
 * size of the patch area data segment.
 */
#define LOS_ERRNO_LP_DATA_OVER_FLOW LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x0d)

/**
 * @ingroup los_lp
 * LP error code: The start address of the static patch unit data segment is smaller than the start address of the
 * patch area data segment, or the static patch unit data segment exceeds the range of the patch area data segment.
 *
 * Value: 0x0200420e.
 *
 * Solution: Check the start address of the static patch unit data segment or increase the
 * size of the patch area data segment.
 */
#define LOS_ERRNO_LP_FUNC_OVERFLOW LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x0e)

/**
 * @ingroup los_lp
 * LP error code: The length of the LP file in the input parameter is incorrect.
 *
 * Value: 0x0200420f.
 *
 * Solution: Check whether the LP file is correct or create a new LP file.
 */
#define LOS_ERRNO_LP_FILE_IS_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x0f)

/**
 * @ingroup los_lp
 * LP error code: The specified management group information is not initialized.
 *
 * Value: 0x02004210.
 *
 * Solution: Check whether the management group is not initialized or fails to be initialized.
 */
#define LOS_ERRNO_LP_MANAGE_UNIT_NO_INIT LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x10)

/**
 * @ingroup los_lp
 * LP error code: The flag of the LP packet is incorrect. It is not equal to 'SP'.
 *
 * Value: 0x02004211.
 *
 * Solution: Check whether the version is correct and whether the LP flag is 'SP'.
 */
#define LOS_ERRNO_LP_IDENT_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x11)

/**
 * @ingroup los_lp
 * LP error code: The CRC verification of the LP file fails.
 *
 * Value: 0x02004212.
 *
 * Solution: Check whether the operation is correct.
 */
#define LOS_ERRNO_LP_PACKET_CRC_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x12)

/**
 * @ingroup los_lp
 * LP error code: The buffer address (buf) passed in during the LP load operation or when obtaining the maximum
 * LP unit number in the LP file is not 4-byte aligned.
 *
 * Value: 0x02004213.
 *
 * Solution: Ensure that the input parameter buffer address (buf) is 4-byte aligned.
 */
#define LOS_ERRNO_LP_BUF_ADDR_NOT_ALIGN LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x13)

/**
 * @ingroup los_lp
 * LP error code: The configured patch area or segment area (start address and length) is not aligned based on
 * cacheline, or the private patch area is not aligned based on 4-byte.
 *
 * Value: 0x02004214.
 *
 * Solution: Reconfigure the start address and length of the patch area or segment area.
 */
#define LOS_ERRNO_LP_REG_NOT_ALIGN LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x14)

/**
 * @ingroup los_lp
 * LP error code: The code segment in the static patch area is incorrectly configured.
 *
 * Value: 0x02004215.
 *
 * Solution: The start address and length of the code segment in the static patch area cannot be 0.
 */
#define LOS_ERRNO_LP_CFG_CODE_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x15)

/**
 * @ingroup los_lp
 * LP error code: The shared data segment in the static patch area is incorrectly configured.
 *
 * Value: 0x02004216.
 *
 * Solution: The start address and length of the shared data segment in the static patch area cannot be 0.
 */
#define LOS_ERRNO_LP_CFG_SDATA_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x16)

/**
 * @ingroup los_lp
 * LP error code: Failed to apply for the control block of the LP unit.
 *
 * Value: 0x02004217.
 *
 * Solution: Check whether the number of loaded LP units is greater than the maximum number of LP units.
 */
#define LOS_ERRNO_LP_PATCH_INDEX_ERROR LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x17)

/**
 * @ingroup los_lp
 * LP error code: One patch package contains multiple patch units.
 *
 * Value: 0x02004218.
 *
 * Solution: Check whether the patch unit is an independent patch unit.
 */
#define LOS_ERRNO_LP_UNIT_NUM_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x18)

/**
 * @ingroup los_lp
 * LP error code: The LP management group ID is unavailable.
 *
 * Value: 0x02004219.
 *
 * Solution: The group ID of the LP management information cannot be greater than or equal to the maximum
 * number of LP management groups.
 */
#define LOS_ERRNO_LP_GROUP_ID_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x19)

/**
 * @ingroup los_lp
 * LP error code: In a static patch group, the code segment in a single partition, shared data segment,
 * and private data segments overlap.
 *
 * Value: 0x0200421a.
 *
 * Solution: Check whether code segments, shared data segments, and private data segments in a single
 * partition of the static patch overlap.
 */
#define LOS_ERRNO_LP_SP_AREA_OVERLAY_IN_SAME_AREA LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x1a)

/**
 * @ingroup los_lp
 * LP error code: The LP unit fails to be activated.
 *
 * Value: 0x0200421b.
 *
 * Solution: Check whether the status of the LP unit is not #UNIT_STATE_DEACTIVATED or #UNIT_STATE_ACTIVATING.
 */
#define LOS_ERRNO_LP_ACTIVE_FAIL LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x1b)

/**
 * @ingroup los_lp
 * LP error code: The LP unit fails to be deactivated.
 *
 * Value: 0x0200421c.
 *
 * Solution: Check whether the status of the LP unit is not #UNIT_STATE_ACTIVATED or #UNIT_STATE_DEACTIVING.
 */
#define LOS_ERRNO_LP_DEACTIVE_FAIL LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x1c)

/**
 * @ingroup los_lp
 * LP error code: The address of the function for obtaining the replaced patch is invalid.
 *
 * Value: 0x0200421d.
 *
 * Solution: Ensure that the patch file is correctly prepared by the tool.
 */
#define LOS_ERRNO_LP_PAT_FUCN_ADDR_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_LP, 0x1d)

/**
 * @ingroup los_lp
 * LP Unit Status Type.
 */
enum LpState {
    UNIT_STATE_ACTIVATED = 0x01,    /**< Indicates the activation status. */
    UNIT_STATE_DEACTIVATED = 0x02,  /**< Indicates the deactivated state. */
    UNIT_STATE_IDLE = 0x03,         /**< Idle state. */
    UNIT_STATE_ACTIVATING = 0x04,   /**< Activation process state. */
    UNIT_STATE_DEACTIVATING = 0x05, /**< Deactivation process state. */
    UNIT_STATE_BUTT                 /**< Invalid state. */
};

/**
 * @ingroup los_lp
 * @brief Patch loading function.
 *
 * @par Description:
 * This API is used to load the LP file in the buffer to a specified patch area or segment.
 * @attention
 * <ul>
 * <li>The size of the patched function must be greater than or equal to 16 bytes.</li>
 * <li>Supports only single-threaded operations, and it is advised to invoke this interface in idle tasks.</li>
 * </ul>
 *
 * @param lpBuf        [IN] Start address of the loading package of the LP unit.
 * @param length       [IN] Size of the loading package of the LP unit.
 *
 * @retval   #LOS_OK                               The patch is loaded successfully.
 * @retval   #LOS_ERRNO_LP_BUF_ADDR_NOT_ALIGN      The buffer address (buf) is not 4-byte aligned.
 * @retval   #LOS_ERRNO_LP_GROUP_ID_INVALID        Invalid LP management group ID.
 * @retval   #LOS_ERRNO_LP_MANAGE_UNIT_NO_INIT     The specified LP management information group is not initialized.
 * @retval   #LOS_ERRNO_LP_INPUT_IS_NULL           The input parameter is null.
 * @retval   #LOS_ERRNO_LP_BUF_NOT_ENOUGH          The patch buffer length is incorrect.
 * @retval   #LOS_ERRNO_LP_IDENT_INVALID           The LP file is invalid.
 * @retval   #LOS_ERRNO_LP_FILE_IS_INVALID         The length of the LP file is incorrect.
 * @retval   #LOS_ERRNO_LP_LOADED                  The patch file is repeatedly loaded.
 * @retval   #LOS_ERRNO_LP_FUNC_OVERFLOW           The number of patch functions to be loaded to a
 * static patch exceeds the maximum.
 * @retval   #LOS_ERRNO_LP_CODE_OVER_FLOW          The address of the static patch unit code segment exceeds
 * the range of the patch area code segment.
 * @retval   #LOS_ERRNO_LP_DATA_OVER_FLOW          The address of the static patch unit data segment exceeds
 * the range of the patch area data segment.
 * @retval   #LOS_ERRNO_LP_PACKET_CRC_INVALID      The CRC check of the LP unit fails.
 * @retval   #LOS_ERRNO_LP_UNIT_NUM_INVALID        The patch file contains multiple patch units.
 * @retval   #LOS_ERRNO_LP_SP_FLAG_INVALID         The flag header in the patch file is not a static patch flag.
 * @retval   #LOS_ERRNO_LP_NO_INVALID              The patch unit number is incorrect.
 * @retval   #LOS_ERRNO_LP_CODE_ADDR_NOT_SUIT      The code segment in the patch file is inconsistent with
 * that in the configuration.
 * @retval   #LOS_ERRNO_LP_DATA_ADDR_NOT_SUIT      The data segment in the patch file is inconsistent with
 * that in the configuration.
 * @retval   #LOS_ERRNO_LP_PATCH_INDEX_ERROR       Failed to apply for the patch unit control block.
 * @retval   #LOS_ERRNO_LP_MEM_NOT_ENOUGH          Memory application failed.
 * @retval   #LOS_ERRNO_LP_PAT_FUCN_ADDR_INVALID   The address of the patch function to be replaced is invalid.
 * @par Dependency:
 * <ul><li>los_lp.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_LpUnitRemove
 * @since Huawei LiteOS RTOS_Lite 208.8.0
 */
extern UINT32 LOS_LpUnitFetch(const UINT8 *lpBuf, UINT32 length);

/**
 * @ingroup los_lp
 * @brief Patch removal function.
 *
 * @par Description:
 * This API is used to delete a specified patch unit.
 * @attention
 * <ul>
 * <li>If the deletion fails, the deleted dependent units will not be rolled back.</li>
 * <li>Supports only single-threaded operations, and it is advised to invoke this interface in idle tasks.</li>
 * </ul>
 *
 * @param unitNo       [IN] User-specified unit number.
 *
 * @retval   #LOS_OK                               Patch unit removed successfully.
 * @retval   #LOS_ERRNO_LP_GROUP_ID_INVALID        Invalid LP management group ID.
 * @retval   #LOS_ERRNO_LP_MANAGE_UNIT_NO_INIT     The LP management information group is not initialized.
 * @retval   #LOS_ERRNO_LP_NOT_LOADED              The patch unit is not loaded.
 * @retval   #LOS_ERRNO_LP_NO_INVALID              The patch unit number is incorrect.
 * @retval   #LOS_ERRNO_LP_DEACTIVE_FAIL           Failed to deactivate the LP unit.
 * @par Dependency:
 * <ul><li>los_lp.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_LpUnitFetch
 * @since Huawei LiteOS RTOS_Lite 208.8.0
 */
extern UINT32 LOS_LpUnitRemove(UINT32 unitNo);

/**
 * @ingroup los_lp
 * @brief Patch activation function.
 *
 * @par Description:
 * This API is used to activate the patch unit.
 * @attention
 * <ul>
 * <li>Replacing the Function of the Activation Operation.</li>
 * <li>Supports only single-threaded operations, and it is advised to invoke this interface in idle tasks.</li>
 * </ul>
 *
 * @param unitNo       [IN] User-specified unit number.
 *
 * @retval   #LOS_OK                               The patch unit is activated successfully.
 * @retval   #LOS_ERRNO_LP_GROUP_ID_INVALID        Invalid LP management group ID.
 * @retval   #LOS_ERRNO_LP_NO_INVALID              The patch unit number is incorrect.
 * @retval   #LOS_ERRNO_LP_MANAGE_UNIT_NO_INIT     The LP management information group is not initialized.
 * @retval   #LOS_ERRNO_LP_NOT_LOADED              The patch unit is not loaded.
 * @retval   #LOS_ERRNO_LP_IS_ACTIVE               The patch unit has been activated.
 * @retval   #LOS_ERRNO_LP_ACTIVE_FAIL             Failed to activate the LP unit.
 * @par Dependency:
 * <ul><li>los_lp.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_LpUnitDeactive
 * @since Huawei LiteOS RTOS_Lite 208.8.0
 */
extern UINT32 LOS_LpUnitActive(UINT32 unitNo);

/**
 * @ingroup los_lp
 * @brief Patch Deactivation Function.
 *
 * @par Description:
 * This API is used to deactivate a specified patch unit.
 * @attention
 * <ul>
 * <li>Patch deactivation is to replace functions.</li>
 * <li>Supports only single-threaded operations, and it is advised to invoke this interface in idle tasks.</li>
 * </ul>
 *
 * @param unitNo       [IN] User-specified unit number.
 *
 * @retval   #LOS_OK                               The patch unit is deactivated successfully.
 * @retval   #LOS_ERRNO_LP_GROUP_ID_INVALID        Invalid LP management group ID.
 * @retval   #LOS_ERRNO_LP_NO_INVALID              The patch unit number is incorrect.
 * @retval   #LOS_ERRNO_LP_MANAGE_UNIT_NO_INIT     The LP management information group is not initialized.
 * @retval   #LOS_ERRNO_LP_NOT_LOADED              The patch unit is not loaded.
 * @retval   #LOS_ERRNO_LP_IS_DEACTIVE             The patch unit is in the deactivated state.
 * @retval   #LOS_ERRNO_LP_INPUT_IS_NULL           The input parameter pointer is empty.
 * @retval   #LOS_ERRNO_LP_DEACTIVE_FAIL           Failed to deactivate the LP unit.
 * @par Dependency:
 * <ul><li>los_lp.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_LpUnitActive
 * @since Huawei LiteOS RTOS_Lite 208.8.0
 */
extern UINT32 LOS_LpUnitDeactive(UINT32 unitNo);

/**
 * @ingroup los_lp
 * @brief Querying the Patch Unit Status.
 *
 * @par Description:
 * This API is used to query the status of a specified patch unit.
 *
 * @param unitNo       [IN] User-specified unit number.
 * @param unitState    [OUT] The patch status to be read cannot be null.
 *
 * @retval   #LOS_OK                               Obtaining the patch unit status succeeded.
 * @retval   #LOS_ERRNO_LP_GROUP_ID_INVALID        Invalid LP management group ID.
 * @retval   #LOS_ERRNO_LP_MANAGE_UNIT_NO_INIT     The LP management information group is not initialized.
 * @retval   #LOS_ERRNO_LP_INPUT_IS_NULL           The pointer for saving the patch status is null.
 * @retval   #LOS_ERRNO_LP_NOT_LOADED              The LP unit is not loaded.
 * @retval   #LOS_ERRNO_LP_NO_INVALID              The patch unit number is incorrect.
 * @par Dependency:
 * <ul><li>los_lp.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS RTOS_Lite 208.8.0
 */
extern UINT32 LOS_LpUnitStateGet(UINT32 unitNo, enum LpState *unitState);

#endif /* LOS_LP_H */
