/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: sec_storage header for SELiteOS.
 */

#ifndef SEC_STORAGE_H
#define SEC_STORAGE_H

#include <errcode.h>
#include "data_crypto.h"

#define IOC_TYPE_SEC_STORAGE            'v'
#define RESERVE_BLOCK_LEN               168  // reserve block is for other use shared_memory
#define SEC_STORAGE_LENGTH_BYTES        2
#define SEC_STORAGE_ID_RANGE            0x100
#define SEC_STORAGE_ID_BASE             0x4000
#define PSA_STORAGE_ID_RANGE            (SEC_STORAGE_ID_RANGE / 2)
#define SEC_STORAGE_AAD_LENGTH          (SEC_STORAGE_HEADER_LENGTH - SEC_STORAGE_IV_LENGTH)
#define MAX_SEC_STORAGE_WRITE_LENGTH    (SHARED_DATA_TEE_ALLOC_SIZE - SEC_STORAGE_LENGTH_BYTES - RESERVE_BLOCK_LEN)
#define MAX_SEC_STORAGE_DATA_LENGTH     \
    (MAX_SEC_STORAGE_WRITE_LENGTH - SEC_STORAGE_HEADER_LENGTH - SEC_STORAGE_TAG_LENGTH)

typedef struct {
    uuid_t uuid;
    uint16_t sec_storage_id_start;
    uint16_t sec_storage_id_length;
} sec_storage_id_range_t;

#ifdef LOSCFG_SUPPORT_TA
void register_ta_sec_storage_id_range(uuid_t *uuid, uint32_t sec_storage_id_start, uint32_t sec_storage_id_length,
                                      uint32_t index);
#endif

typedef struct {
    uint8_t permanent;   /*!< @if Eng Indicates whether the sec_storage data is permanent.
                           @else   是否为永久sec_storage @endif */
    uint8_t encrypted;   /*!< @if Eng Indicates whether to encrypt again in REE.
                           @else   REE侧是否再加密一次数据 @endif */
    uint8_t non_upgrade; /*!< @if Eng Indicates whether upgradeable.
                           @else   是否不可升级 @endif */
    uint8_t reserve;     /*!< @if Eng Reserved field for the future
                              @else   保留字段 @endif */
} sec_storage_attr_t;

/**
 * @if Eng
 * @brief  Store a sec_storage data.
 * @par Description: Writes sec_storage data item. The default attribute is Normal and no callback function is
 * available.
 * @param  [in]  sec_storage_id The sec_storage_id to which the data is associated.
 * @param  [in]  data The value to store.
 * @param  [in]  data_length Length of data in bytes.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li sec_storage.h
 * @else
 * @brief  写入sec_storage数据项
 * @par 说明: 默认属性为Normal，没有回调函数。
 * @param  [in]  sec_storage_id 要写入的sec_storage项的sec_storage_id，用于索引
 * @param  [in]  data 指向要写入的sec_storage项的值的指针
 * @param  [in]  data_length 写入数据的长度（以字节为单位）
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li sec_storage.h
 * @endif
 */
errcode_t uapi_sec_storage_write(uint16_t sec_storage_id, const uint8_t *data, uint16_t data_length);

/**
 * @if Eng
 * @brief  Get a value associated with a specific sec_storage data.
 * @par Description: Reads sec_storage data.
 * @attention By default, the sec_storage attribute value is not obtained.
 * @param  [in]  sec_storage_id The sec_storage_id to which the data is associated.
 * @param  [in]  data_max_length The maximum length, in bytes,
 *                               that is allowed to be copied in the data buffer if the sec_storage_id is found.
 * @param  [out] data_length Length of data in bytes.
 * @param  [out] data The value to store.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li sec_storage.h
 * @else
 * @brief  读取指定sec_storage数据项的值
 * @par 说明: 读取指定sec_storage数据项的值
 * @attention 默认情况下，不获取sec_storage属性值
 * @param  [in]  sec_storage_id 要读取的sec_storage项的sec_storage_id，用于索引
 * @param  [in]  data_max_length 允许存储数据的最大长度（以字节为单位）
 * @param  [out] data_length 实际读取到的数据长度
 * @param  [out] data 指向保存读取数据的buffer的指针
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li sec_storage.h
 * @endif
 */
errcode_t uapi_sec_storage_read(uint16_t sec_storage_id, uint16_t data_max_length, uint16_t *data_length,
                                uint8_t *data);

/**
 * @if Eng
 * @brief  Store a sec_storage data with attribute.
 * @par Description: Writes sec_storage data item with attribute and no callback function is available.
 * @param  [in]  sec_storage_id The sec_storage_id to which the data is associated.
 * @param  [in]  data The value to store.
 * @param  [in]  data_length Length of data in bytes.
 * @param  [in]  attr Attribute of data.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li sec_storage.h
 * @else
 * @brief  写入带属性的sec_storage数据项
 * @par 说明: 默认属性为Normal，没有回调函数。
 * @param  [in]  sec_storage_id 要写入的sec_storage项的sec_storage_id，用于索引
 * @param  [in]  data 指向要写入的sec_storage项的值的指针
 * @param  [in]  data_length 写入数据的长度（以字节为单位）
 * @param  [in]  attr 写入数据的属性
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li sec_storage.h
 * @endif
 */
errcode_t uapi_sec_storage_write_with_attr(uint16_t sec_storage_id, const uint8_t *data, uint16_t data_length,
                                           sec_storage_attr_t attr);

/**
 * @if Eng
 * @brief  delete a sec_storage data with attribute.
 * @par Description: delete sec_storage data item
 * @param  [in]  sec_storage_id The sec_storage_id to which the data is associated.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li sec_storage.h
 * @else
 * @brief  删除sec_storage数据项
 * @par 说明: 默认属性为Normal
 * @param  [in]  sec_storage_id 要删除的sec_storage项的sec_storage_id，用于索引
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li sec_storage.h
 * @endif
 */
errcode_t uapi_sec_storage_delete(uint16_t sec_storage_id);

/**
 * @if Eng
 * @brief  get sec_storage length.
 * @par Description: The default attribute is Normal.
 * @param  [in]  sec_storage_id The sec_storage_id to which the data is associated.
 * @param  [out] sec_storage length value.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li sec_storage.h
 * @else
 * @brief  读取sec_storage的长度
 * @par 说明: 默认属性为Normal
 * @param  [in]  sec_storage_id 要获取长度的sec_storage项的sec_storage_id，用于索引
 * @param  [out] sec_storage的buffer长度
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li sec_storage.h
 * @endif
 */
errcode_t uapi_sec_storage_get_key_length(uint16_t sec_storage_id, uint16_t *length);
#endif
