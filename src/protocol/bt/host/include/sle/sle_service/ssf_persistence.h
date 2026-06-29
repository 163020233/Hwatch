/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Service Framework (SSF) about Persistence API File.
 */
#ifndef SSF_PERSISTENCE_H
#define SSF_PERSISTENCE_H
#include <stdint.h>
#include "errcode.h"
#include "sle_common.h"
#ifdef __cplusplus
extern "C" {
#endif

#define SSF_PERSISTENCE_MAX_DEVICE_NAME 32
#define SSF_PERSISTENCE_MAX_DEVICE_NUM 3
#define SSF_PERSISTENCE_MAX_PDL_NUM 10
#define SSF_PERSISTENCE_LINK_KEY_LEN 16

/**
 * @if Eng
 * @brief Type of SLE persistence write to NV.
 * @else
 * @brief 星闪持久化写nv的类型。
 * @endif
 */
typedef enum {
    WRITE_SINGLE_PDL_ADD,
    WRITE_SINGLE_PDL_UPDATE,
} bs_sle_write_single_pdl_to_nv_reason_type;

/**
 * @if Eng
 * @brief  SLE NV persistence single device information.
 * @else
 * @brief  星闪NV持久化单条设备信息。
 * @endif
 */
typedef struct {
    uint8_t device_addr[SLE_ADDR_LEN];  /*!< @if Eng Device Address
                                             @else   设备地址 @endif */
    uint8_t link_key[SLE_LINK_KEY_LEN]; /*!< @if Eng link key
                                             @else   链路密钥 @endif */
    uint8_t crypto_algo;                /*!< @if Eng encryption algorithm type { @ref sle_crypto_algo_t }
                                             @else   加密算法类型 { @ref sle_crypto_algo_t } @endif */
    uint8_t key_deriv_algo;             /*!< @if Eng key distribution algorithm type { @ref sle_key_deriv_algo_t }
                                             @else   秘钥分发算法类型 { @ref sle_key_deriv_algo_t } @endif */
    uint8_t integr_chk_ind;             /*!< @if Eng integrity check indication { @ref sle_integr_chk_ind_t }
                                             @else   完整性校验指示 { @ref sle_integr_chk_ind_t } @endif */
    uint8_t is_bond;                    /*!< @if Eng pair bonding indication { @ref sle_bond_ind_t }
                                             @else   配对绑定指示 { @ref sle_bond_ind_t } @endif */
    char device_name[SLE_NAME_MAX_LEN]; /*!< @if Eng Device Name
                                             @else   设备名称 @endif */
    uint8_t call_speaker_volume;        /*!< @if Eng Call Speaker Volume
                                             @else   通话扬声器音量 @endif */
    uint8_t media_volume;               /*!< @if Eng Media Volume
                                             @else   媒体音量 @endif */
    uint8_t channel;                    /*!< @if Eng Audio Channel
                                             @else   音乐声道 @endif */
    uint8_t reserved[29];               /*!< @if Eng Reserved 30-byte field
                                             @else   预留26字节字段 @endif */
} bs_sle_nv_device_data_t;

/**
 * @if Eng
 * @brief SLE NV persistence object information.
 * @else
 * @brief 星闪NV持久化obj对象信息。
 * @endif
 */
typedef struct {
    uint8_t device_num;                                                  /*!< @if Eng Device Number In Obj
                                                                              @else   Obj中设备数量 @endif */
    bs_sle_nv_device_data_t device_data[SSF_PERSISTENCE_MAX_DEVICE_NUM]; /*!< @if Eng Device Information
                                                                              @else   设备信息 @endif */
    uint8_t reserved[32];                                                /*!< @if Eng Reserved 32-byte field
                                                                              @else   预留32字节字段 @endif */
} bs_sle_nv_obj_data_t;

/**
 * @if Eng
 * @brief SLE NV persistence data information.
 * @else
 * @brief 星闪NV持久化单条数据信息
 * @endif
 */
typedef struct {
    uint8_t index;                                        /*!< @if Eng Index of nv data.
                                                                @else   存储数据索引 @endif */
    uint8_t pdl_num;                                      /*!< @if Eng Total number of stored data entries,
                                                                not exceeding SSF_PERSISTENCE_MAX_PDL_NUM
                                                               @else   总存储数据条数，不大于于SSF_PERSISTENCE_MAX_PDL_NUM @endif */
    bs_sle_write_single_pdl_to_nv_reason_type write_type; /*!< @if Eng Reason of write nv data.
                                                               @else   写数据原因 @endif */
    bs_sle_nv_obj_data_t pdl_data;                        /*!< @if Eng obj data to nv.
                                                               @else   写obj数据 @endif */
} bs_sle_nv_single_pdl_data_t;

/**
 * @if Eng
 * @brief SLE NV persistence Connection Sequence Information
 * @else
 * @brief 星闪NV持久化连接顺序信息
 * @endif
 */
typedef struct {
    uint8_t pdl_num;                                     /*!< @if Eng Total number of stored data entries,
                                                               not exceeding SSF_PERSISTENCE_MAX_PDL_NUM
                                                              @else   总存储数据条数，不大于于SSF_PERSISTENCE_MAX_PDL_NUM @endif */
    uint8_t pdl_conn_order[SSF_PERSISTENCE_MAX_PDL_NUM]; /*!< @if Eng Connection sequence of stored data entries
                                                              @else   存储数据条目的连接顺序 @endif */
} bs_sle_nv_conn_order_t;

/**
 * @if Eng
 * @brief  Callback function for reporting a single pdl.
 * @par    After registering this callback function, the upper layer will be notified
 *         when the data that needs to be persisted changes.
 * @param  [in]  bs_sle_nv_single_pdl_data_t    single pdl data.
 * @par Dependency:
 * @see bs_sle_persistence_callbacks_t
 * @else
 * @brief  上报单条pdl的回调函数。
 * @par    注册该回调函数之后，需要持久化的数据发生变化时会上报上层。
 * @param  [in]  bs_sle_nv_single_pdl_data_t    单条pdl数据。
 * @par 依赖:
 * @see bs_sle_persistence_callbacks_t
 * @endif
 */
typedef void (*bs_sle_pdl_report_callback)(bs_sle_nv_single_pdl_data_t *param);

/**
 * @if Eng
 * @brief  Callback function for reporting connection sequence.
 * @par    After registering this callback function, the upper layer will be notified
 *         when the device connection order changes.
 * @param  [in]  bs_sle_nv_conn_order_t    Connect sequential data.
 * @par Dependency:
 * @see bs_sle_persistence_callbacks_t
 * @else
 * @brief  上报连接顺序的回调函数。
 * @par    注册该回调函数之后，当设备连接顺序发生变化时会上报上层。
 * @param  [in]  bs_sle_nv_conn_order_t    连接顺序数据。
 * @par 依赖:
 * @see bs_sle_persistence_callbacks_t
 * @endif
 */
typedef void (*bs_sle_connect_order_report_callback)(bs_sle_nv_conn_order_t *param);

/**
 * @if Eng
 * @brief Struct of sle service persistence callback function.
 * @else
 * @brief sle service持久化回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_sle_pdl_report_callback single_pdl_report_cb;              /*!< @if Eng  Callback function for reporting
                                                                        a single pdl data.
                                                                       @else   单条pdl数据回调 @endif */
    bs_sle_connect_order_report_callback connect_order_report_cb; /*!< @if Eng  Callback function for reporting
                                                                        connection sequence data.
                                                                       @else   连接顺序数据回调 @endif */
} bs_sle_persistence_callbacks_t;

/**
 * @if Eng
 * @brief  Register the callback function of sle service persistence.
 * @par    The callback function is used to report the data that needs to be persisted.
 * @param  [in]  bs_sle_persistence_callbacks_t    Struct of callback function.
 * @retval errcode_t ERRCODE_SUCC on success, error code on failure.
 * @par Dependency:
 * @see bs_sle_persistence_unregister_callbacks
 * @else
 * @brief  注册sle service持久化回调函数。
 * @par    该回调函数用于上报需要持久化的数据。
 * @param  [in]  bs_sle_persistence_callbacks_t    回调函数接口。
 * @retval errcode_t 成功时返回ERRCODE_SUCC，失败时返回错误码。
 * @par 依赖:
 * @see bs_sle_persistence_unregister_callbacks
 * @endif
 */
errcode_t bs_sle_persistence_register_callbacks(bs_sle_persistence_callbacks_t *func);

/**
 * @if Eng
 * @brief  Unregister the callback function of sle service persistence.
 * @retval errcode_t ERRCODE_SUCC on success, error code on failure.
 * @par Dependency:
 * @see bs_sle_persistence_register_callbacks
 * @else
 * @brief  取消注册sle service持久化回调函数。
 * @retval errcode_t 成功时返回ERRCODE_SUCC，失败时返回错误码。
 * @par 依赖:
 * @see bs_sle_persistence_register_callbacks
 * @endif
 */
errcode_t bs_sle_persistence_unregister_callbacks(void);

/**
 * @if Eng
 * @brief  Config pdl data to service.
 * @param  [in]  pdl_cnt    Total number of stored data entries, not exceeding SSF_PERSISTENCE_MAX_PDL_NUM.
 * @param  [in]  pdl_data   Struct of pdl data.
 * @retval errcode_t ERRCODE_SUCC on success, error code on failure.
 * @else
 * @brief  配置pdl数据到service。
 * @param  [in]  pdl_cnt    总存储数据条数，不大于于SSF_PERSISTENCE_MAX_PDL_NUM。
 * @param  [in]  pdl_data   pdl数据接口。
 * @retval errcode_t 成功时返回ERRCODE_SUCC，失败时返回错误码。
 * @endif
 */
errcode_t bs_sle_config_pdl_data(uint8_t pdl_cnt, bs_sle_nv_obj_data_t *pdl_data);

/**
 * @if Eng
 * @brief  Config connection sequence data to service.
 * @param  [in]  order    Struct of connection sequence data.
 * @retval errcode_t ERRCODE_SUCC on success, error code on failure.
 * @else
 * @brief  配置连接顺序数据到service。
 * @param  [in]  order    连接顺序数据接口。
 * @retval errcode_t 成功时返回ERRCODE_SUCC，失败时返回错误码。
 * @endif
 */
errcode_t bs_sle_config_connect_order(bs_sle_nv_conn_order_t *order);

#ifdef __cplusplus
}
#endif
#endif /* SSF_PERSISTENCE_H */