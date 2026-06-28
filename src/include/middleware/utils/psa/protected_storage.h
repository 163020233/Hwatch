/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * filename:psa_protected_storage.h
 * Author: @CompanyNameTag
 * Create:2025-02-20
 * Description: Describes the Protected Storage API
 */

#ifndef PSA_PROTECTED_STORAGE_H
#define PSA_PROTECTED_STORAGE_H

#include <stddef.h>
#include <stdint.h>

#include "psa/psa_error.h"
#include "psa/psa_storage_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  The major version number of the Protected Storage API.
 * @else
 * @brief  安全存储API的主版本号。
 * @endif
 */
#define PSA_PS_API_VERSION_MAJOR 1

/**
 * @if Eng
 * @brief  The major version number of the Protected Storage API.
 * @else
 * @brief  安全存储API的次要版本号。
 * @endif
 */
#define PSA_PS_API_VERSION_MINOR 0

/**
 * @if Eng
 * @brief  Set the data associated with the specified uid.
 * @param  [in] uid          The identifier for the data.
 * @param  [in] data_length  The size in bytes of the data in p_data.
 * @param  [in] p_data       A buffer containing the data.
 * @param  [in] create_flags The flags indicating the properties of the data.
 * @retval PSA_SUCCESS Success
 * @retval Other Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  根据特定uid存储数据
 * @param  [in] uid          数据id
 * @param  [in] data_length  数据长度
 * @param  [in] p_data       数据缓存区
 * @param  [in] create_flags 数据标志属性
 * @retval PSA_SUCCESS 成功
 * @retval Other 失败， 参考 @ref psa_status_t.
 * @endif
 */
psa_status_t psa_ps_set(psa_storage_uid_t uid, size_t data_length, const void *p_data,
                        psa_storage_create_flags_t create_flags);

/**
 * @if Eng
 * @brief Retrieve data associated with a provided uid.
 * @param  [in]  uid            The uid value.
 * @param  [in]  data_offset    The starting offset of the data requested.
 * @param  [in]  data_size      The amount of data requested.
 * @param  [out] p_data         On success, the buffer where the data will be placed.
 * @param  [out] p_data_length  On success, will contain size of the data placed in
 *                              p_data.
 * @retval PSA_SUCCESS Success
 * @retval Other Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  获取特定uid存储数据
 * @param  [in]  uid              数据id
 * @param  [in]  data_offset      数据偏移量
 * @param  [in]  data_size        数据大小
 * @param  [out] p_data           输出数据缓存区
 * @param  [out] p_data_length    输出数据大小
 * @retval PSA_SUCCESS 成功
 * @retval Other 失败， 参考 @ref psa_status_t.
 * @endif
 */
psa_status_t psa_ps_get(psa_storage_uid_t uid, size_t data_offset, size_t data_size, void *p_data,
                        size_t *p_data_length);

/**
 * @if Eng
 * @brief Retrieve the metadata about the provided uid.
 * @param  [in]  uid     The identifier for the data.
 * @param  [out] p_info  A pointer to the psa_storage_info_t struct that will be
 *                       populated with the metadata.
 * @retval PSA_SUCCESS Success
 * @retval Other Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  根据特定uid获取元数据
 * @param  [in]  uid              数据id
 * @param  [out] p_info           元数据输出缓存区
 * @retval PSA_SUCCESS 成功
 * @retval Other 失败， 参考 @ref psa_status_t.
 * @endif
 */
psa_status_t psa_ps_get_info(psa_storage_uid_t uid, struct psa_storage_info_t *p_info);

/**
 * @if Eng
 * @brief  Remove the provided uid and its associated data from the storage.
 * @param  [in] uid          The identifier for the data.
 * @retval PSA_SUCCESS Success
 * @retval Other Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  根据特定uid删除数据
 * @param  [in] uid          数据id
 * @retval PSA_SUCCESS 成功
 * @retval Other 失败， 参考 @ref psa_status_t.
 * @endif
 */
psa_status_t psa_ps_remove(psa_storage_uid_t uid);

/**
 * @if Eng
 * @brief  Reserves storage for the specified uid.
 * @param  [in] uid          The identifier for the data.
 * @param  [in] capacity     The allocated capacity, in bytes, of the uid.
 * @param  [in] create_flags The flags indicating the properties of the data.
 * @retval PSA_SUCCESS Success
 * @retval Other Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  根据特定uid存储数据
 * @param  [in] uid           数据id
 * @param  [in] capacity      uid数据容量
 * @param  [in] create_flags  数据标志属性
 * @retval PSA_SUCCESS 成功
 * @retval Other 失败， 参考 @ref psa_status_t.
 * @endif
 */
psa_status_t psa_ps_create(psa_storage_uid_t uid, size_t capacity, psa_storage_create_flags_t create_flags);

/**
 * @if Eng
 * @brief Retrieve data associated with a provided uid.
 * @param  [in]  uid           The uid value.
 * @param  [in]  data_offset   The starting offset of the data requested.
 * @param  [in]  data_length   The amount of data requested.
 * @param  [out] p_data        On success, the buffer where the data will be placed.
 * @retval PSA_SUCCESS Success
 * @retval Other Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  获取特定uid存储数据
 * @param  [in]  uid              数据id
 * @param  [in]  data_offset      数据偏移量
 * @param  [in]  data_length      数据大小
 * @param  [out] p_data           输出数据缓存区
 * @retval PSA_SUCCESS 成功
 * @retval Other 失败， 参考 @ref psa_status_t.
 * @endif
 */
psa_status_t psa_ps_set_extended(psa_storage_uid_t uid, size_t data_offset, size_t data_length, const void *p_data);

/**
 * @if Eng
 * @brief  Returns a bitmask with flags set for the optional features supported
 *         by the implementation.
 * @retval PSA_STORAGE_SUPPORT_SET_EXTENDED Success
 * @retval Other Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  返回一个位掩码，其中设置了实现所支持的可选功能的标志。
 * @retval PSA_STORAGE_SUPPORT_SET_EXTENDED 成功
 * @retval Other 失败， 参考 @ref psa_status_t.
 * @endif
 */
uint32_t psa_ps_get_support(void);

#ifdef __cplusplus
}
#endif

#endif  // PSA_PROTECTED_STORAGE_H
