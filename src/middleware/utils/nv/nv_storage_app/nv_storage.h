/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: KEY VALUE STORAGE INTERNAL INTERFACE
 */

#ifndef NV_STORAGE_H
#define NV_STORAGE_H

#include "nv_notify.h"
#include "nv.h"
#include "errcode.h"
#include "nv_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

nv_direct_ctrl_t *nv_direct_get_nv_ctrl(kv_store_t store);

/**
 * Force store a key value pair, even if the key has been set to permanent.
 * @param key key to associate kvalue to
 * @param kvalue value to store
 * @param kvalue_length length in bytes of kvalue
 * @return SOC_ERR_SUCCESS or an error code
 */
errcode_t uapi_nv_write_force(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length);

/**
 * Update the attributes of an existing key.
 * @param key key to associate make permanent
 * @param attr Sets the storage attributes of a key.
 * @return SOC_ERR_SUCCESS or an error code
 * @note This key can not then be deleted or overwitten, and there is no mechanism to undo this
 */
errcode_t uapi_nv_update_key_attr(uint16_t key, nv_key_attr_t *attr, nv_storage_completed_callback func);

 /**
 * @ingroup iot_nv
 * @brief compare a value associated with a specific key.
 *        CNcomment:检验特定NV的值 CNend
 *
 * @par 描述：
 * Check whether NV data has been stored. If the stored data is the same
 * as the input parameter data,a success message is returned.
 * CNcomment:检查NV数据是否已存储。如果存储的数据与入参数据相同，则返回成功 CNend
 * @attention None.
 *
 * @param key               [IN] The key to which the kvalue is associated.
 *                               CNcomment:要检验的NV ID CNend
 * @param kvalue_length     [IN] Length of kvalue in bytes.
 *                               CNcomment:要检验的NV长度 CNend
 * @param kvalue            [IN] The value to store.CNcomment:待进行对比的NV值 CNend
 *
 * @retval #true                Success
 * @retval #false               Failure
 *
 * @par 依赖：
 * @li nv
 * @li nv.h
 * @see None
 */
bool uapi_nv_is_stored(uint16_t key, uint16_t kvalue_length, const uint8_t *kvalue);


/**
 * @if Eng
 * @brief Initializes the NV store partition specified by the user..
 * @param  [in] store which region is located in. @ref kv_store_t.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief 初始化用户指定的NV store分区。
 * @param  [in]  store 指定NV区域。参考 @ref kv_store_t。
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_extra_init(kv_store_t store);
 
/**
 * @if Eng
 * @brief  Store a key value pair in specify NV store region.
 * @par Description: Writes NV data item. The default attribute is Normal and no callback function is available.
 * @param  [in]  store which region is located in. @ref kv_store_t.
 * @param  [in]  key The key ID to which the kvalue is associated.
 *                   It is independent of key_id in the NV_DATA partition and they can be duplicated.
 * @param  [in]  kvalue The value to store.
 * @param  [in]  kvalue_length Length of kvalue in bytes.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  在 指定NV store 区域写入NV数据项
 * @par 说明: 默认属性为Normal，没有回调函数。
 * @param  [in]  store 指定NV区域。参考 @ref kv_store_t。
 * @param  [in]  key 要写入的NV项的key ID，用于索引。和 NV_DATA 分区中的 key_id 是相互独立的，可以重复的。
 * @param  [in]  kvalue 指向要写入的NV项的值的指针
 * @param  [in]  kvalue_length 写入数据的长度（以字节为单位）
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_extra_write(kv_store_t store, uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length);
 
/**
 * @if Eng
 * @brief  set a value associated with a specific key in NV store region.
 * @par Description: Write the NV data associated with a particular key.
 * @attention The encryption attribute and permanent attribute of a key cannot be modified,
 *            and the kvalue of the permanent attribute cannot be modified.
 * @param  [in]  store which region is located in. @ref kv_store_t.
 * @param  [in]  key The key to which the kvalue is associated.
 *                   It is independent of key_id in the NV_DATA partition and they can be duplicated.
 * @param  [in]  kvalue The value to store.
 * @param  [in]  kvalue_length Length of kvalue in bytes.
 * @param  [in]  attr Set the storage properties of the key.
 * @param  [in]  func Callback function invoked after kvalue is written to the flash memory.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  在 NV store 区域写入NV数据项，并根据业务需求配置属性及回调函数
 * @par 说明: 写入NV数据项，并根据业务需求配置属性及回调函数
 * @attention NV的加密属性和永久属性不能修改，永久属性的kvalue不能修改
 * @param  [in]  store 指定NV区域。参考 @ref kv_store_t。
 * @param  [in]  key 要写入的NV项的key ID，用于索引。和 NV_DATA 分区中的 key_id 是相互独立的，可以重复的。
 * @param  [in]  kvalue 指向要写入的NV项的值的指针
 * @param  [in]  kvalue_length 写入数据的长度（以字节为单位）
 * @param  [in]  attr 要配置的NV项的属性
 * @param  [in]  func kvalue写入flash后调用的回调函数
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_extra_write_with_attr(kv_store_t store, uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length,
                                        nv_key_attr_t *attr, nv_storage_completed_callback func);

/**
 * Force store a key value pair, even if the key has been set to permanent.
 * @param  [in]  store which region is located in. @ref kv_store_t.
 * @param  [in]  key key to associate kvalue to
 * @param  [in]  kvalue value to store
 * @param  [in]  kvalue_length length in bytes of kvalue
 * @return ERRCODE_SUCC or an error code
 */
errcode_t uapi_nv_extra_write_force(kv_store_t store, uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length);

/**
 * Update the attributes of an existing key.
 * @param  [in]  store which region is located in. @ref kv_store_t.
 * @param  [in]  key key to associate make permanent
 * @param  [in]  attr Sets the storage attributes of a key.
 * @return SOC_ERR_SUCCESS or an error code
 * @note This key can not then be deleted or overwitten, and there is no mechanism to undo this
 */
errcode_t uapi_nv_extra_update_key_attr(kv_store_t store, uint16_t key, nv_key_attr_t *attr, nv_storage_completed_callback func);

/**
 * @if Eng
 * @brief  Get a value associated with a specific key in NV store region.
 * @par Description: Reads NV data.
 * @attention By default, the key attribute value is not obtained.
 * @param  [in]  store which region is located in. @ref kv_store_t.
 * @param  [in]  key The key to which the kvalue is associated.
 *                   It is independent of key_id in the NV_DATA partition and they can be duplicated.
 * @param  [in]  kvalue_max_length The maximum length, in bytes,
 *                               that is allowed to be copied in the kvalue buffer if the key is found.
 * @param  [out] kvalue_length Length of kvalue in bytes.
 * @param  [out] kvalue The value to store.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  读取 NV store 区域中指定NV数据项的值
 * @par 说明: 读取指定NV数据项的值
 * @attention 默认情况下，不获取NV属性值
 * @param  [in]  store 指定NV区域。参考 @ref kv_store_t。
 * @param  [in]  key 要读取的NV项的key ID，用于索引。和 NV_DATA 分区中的 key_id 是相互独立的，可以重复的。
 * @param  [in]  kvalue_max_length 允许存储数据的最大长度（以字节为单位）
 * @param  [out] kvalue_length 实际读取到的数据长度
 * @param  [out] kvalue 指向保存读取数据的buffer的指针
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_extra_read(kv_store_t store, uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue);
 
/**
 * @if Eng
 * @brief  Get a value associated with a specific key in NV store region.
 * @par Description: Reads NV data and obtains the attribute value of the key.
 * @param  [in]  store which region is located in. @ref kv_store_t.
 * @param  [in]  key The key to which the kvalue is associated.
 *                   It is independent of key_id in the NV_DATA partition and they can be duplicated.
 * @param  [in]  kvalue_max_length The maximum length, in bytes,
 *                               that is allowed to be copied in the kvalue buffer if the key is found.
 * @param  [out] kvalue_length Length of kvalue in bytes.
 * @param  [out] kvalue The value to store.
 * @param  [out] attr Attribute value of the key.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  读取 NV store 区域中指定NV数据项的值，同时获取key的属性值
 * @par 说明: 读取指定NV数据项的值，同时获取key的属性值
 * @param  [in]  store 指定NV区域。参考 @ref kv_store_t。
 * @param  [in]  key 要读取的NV项的key ID，用于索引。和 NV_DATA 分区中的 key_id 是相互独立的，可以重复的。
 * @param  [in]  kvalue_max_length 允许存储数据的最大长度（以字节为单位）
 * @param  [out] kvalue_length 实际读取到的数据长度
 * @param  [out] kvalue 指向保存读取数据的buffer的指针
 * @param  [out] attr 获取到的NV项的属性
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_extra_read_with_attr(kv_store_t store, uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length,
                                       uint8_t *kvalue, nv_key_attr_t *attr);

/**
 * @if Eng
 * @brief  Get the length and attributes associated with a specific key.
 * @par Description: Get only the length and attributes of the NV Key without reading the NV Key data.
 * @param  [in]  store which region is located in. @ref kv_store_t.
 * @param  [in]  key The key to which the attributes is associated.
 * @param  [out] length Length of NV Key in bytes.
 * @param  [out] attr Attribute value of the key.
 * @retval ERRCODE_SUCC      Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  获取NV_EXTRA 区域中指定NV Key的长度和属性值
 * @par 说明: 只获取指定NV Key的长度和属性值，而不读取NV key的数据
 * @param  [in]  store 指定NV区域。参考 @ref kv_store_t。
 * @param  [in]  key 要读取的NV项的key ID，用于索引。
 * @param  [out] length 获取到的NV项的数据长度
 * @param  [out] attr 获取到的NV项的属性
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_extra_get_key_attr(kv_store_t store, uint16_t key, uint16_t *length, nv_key_attr_t *attr);

/**
 * @if Eng
 * @brief  Delete the specified Key.
 * @par Description: completely deletion refers to physically overwriting the data.
 * @param  [in]  store which region is located in. @ref kv_store_t.
 * @param  [in]  key The key ID to be deleted.
 * @param  [in]  completely Indicate whether it is completely deleted.
 * @retval ERRCODE_SUCC      Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  删除额外分区的指定的NV Key.
 * @par 说明: 彻底擦除指该KEY的数据在物理上进行数据覆盖。
 * @param  [in]  store 指定NV区域。参考 @ref kv_store_t。
 * @param  [in]  key 要删除的NV项的key ID，用于索引
 * @param  [in]  completely 是否彻底删除
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_extra_delete_key(kv_store_t store, uint16_t key, bool completely);

 /**
 * @ingroup iot_nv
 * @brief compare a value associated with a specific key.
 *        CNcomment:检验特定NV的值 CNend
 *
 * @par 描述：
 * Check whether NV data has been stored. If the stored data is the same
 * as the input parameter data,a success message is returned.
 * CNcomment:检查NV数据是否已存储。如果存储的数据与入参数据相同，则返回成功 CNend
 * @attention None.
 *
 * @param store             [IN] The keywhich region is located in.
 *                               CNcomment:指定NV区域
 * @param key               [IN] The key to which the kvalue is associated.
 *                               CNcomment:要检验的NV ID CNend
 * @param kvalue_length     [IN] Length of kvalue in bytes.
 *                               CNcomment:要检验的NV长度 CNend
 * @param kvalue            [IN] The value to store.CNcomment:待进行对比的NV值 CNend
 *
 * @retval #true                Success
 * @retval #false               Failure
 *
 * @par 依赖：
 * @li nv
 * @li nv.h
 * @see None
 */
bool uapi_nv_extra_is_stored(kv_store_t store, uint16_t key, uint16_t kvalue_length, const uint8_t *kvalue);

/**
 * @if Eng
 * @brief  Get the space usage of NV store storage.
 * @par Description: Query the NV space status.
 * @param  [in]  store which region is located in. @ref kv_store_t.
 * @param  [out] status pointer to NV status .
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  获取 NV store 存储的空间使用情况
 * @par 说明: 查询NV空间状态
 * @param  [in]  store 指定NV区域。参考 @ref kv_store_t。
 * @param  [out] status 指向保存NV状态数据的指针
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_extra_get_store_status(kv_store_t store, nv_store_status_t *status);

/**
 * @if Eng
 * @brief  NV backup.
 * @par Description: NV backup.
 * @param  [in] store which region is located in.
 * @param  [in] backup_mode pointer to NV backup region flag.
 * @retval ERRCODE_SUCC      Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  执行NV备份
 * @par 说明: 执行NV备份
 * @param  [in] store       指定NV区域
 * @param  [in] backup_mode  指向保存NV备份区域选择的指针
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_extra_backup(kv_store_t store, const nv_backup_mode_t *backup_mode);

/**
 * @if Eng
 * @brief Set NV all restore flag.
 * @param  [in] store which region is located in.
 * @retval ERRCODE_SUCC      Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief 设置NV全量恢复标志
 * @param  [in] store        指定NV区域
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_extra_set_restore_mode_all(kv_store_t store);

/**
 * @if Eng
 * @brief  Set NV partitial restore flag.
 * @par Description: Set NV partitial restore flag.
 * @param  [in] store which region is located in.
 * @param  [in] restore_mode pointer to NV restore region flag.
 * @retval ERRCODE_SUCC        Success.
 * @retval Other               ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  设置NV部分恢复标志
 * @par 说明: 设置NV部分恢复标志
 * @param  [in] store          指定NV区域
 * @param  [in] restore_mode   指向保存NV各区域是否恢复标志的指针
 * @retval ERRCODE_SUCC        成功返回#ERRCODE_SUCC
 * @retval Other               失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_extra_set_restore_mode_partitial(kv_store_t store, const nv_restore_mode_t *restore_mode);

/**
 * @if Eng
 * @brief  register callback function of nv key value changed notification in NV store region.
 * @param  [in]  store which region is located in. @ref kv_store_t.
 * @param  min_key Minimum key-ID supported by registration callback.
 * @param  max_key Maximum key-ID supported by registration callback.
 * @param  func callback function.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  注册 NV store 区域中nv键值改变通知的回调函数
 * @param  [in]  store 指定NV区域。参考 @ref kv_store_t。
 * @param  min_key 注册回调支持的最小key-ID
 * @param  max_key 注册回调支持的最大key-ID
 * @param  func 回调函数.
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_extra_register_change_notify_proc(kv_store_t store, uint16_t min_key, uint16_t max_key, nv_changed_notify_func func);

/**
 * @if Eng
 * @brief  Ensure NV data from ram flush to flash.
 * @param  [in]  store which region is located in. @ref kv_store_t.
 * @par Description: Only when nv open the CONFIG NV SUPPORT ASYNCHRONOUS STORE valid.
 * @retval ERRCODE_SUCC        Success.
 * @retval Other               ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  确保指定store分区的NV数据从Ram上同步到Falsh
 * @par 说明: 仅NV支持异步存储时调用有效。
 * @retval ERRCODE_SUCC        成功返回#ERRCODE_SUCC
 * @retval Other               失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_extra_flush(kv_store_t store);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* NV_STORAGE_H */
