/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 *
 * Description: BTS BR FACTORY module.
 */
#ifndef BTS_BR_FACTORY_H
#define BTS_BR_FACTORY_H

#include "bts_def.h"
#include "errcode.h"

/**
 * @if Eng
 * @brief Enum of br radio frequency tx start
 * @else
 * @brief br开启射频长发参数
 * @endif
 */
typedef struct {
    uint16_t test_data_len;    /*!< @if Eng tx test data len,Scope:0x00~0x3FD
                                    @else   发射测试数据长度,取值范围:0x00~0x3FD @endif */
    uint8_t addr[BD_ADDR_LEN]; /*!< @if Eng tx device address
                                    @else   对端设备地址 @endif */
    uint8_t tx_freq;           /*!< @if Eng tx frequency,Scope:0x00~0x4E,2402+x*2
                                    @else   发送频率,范围:0x00~0x4E,2402+x*2 @endif */
    uint8_t module_type;       /*!< @if Eng modulation type
                                    @else   调制类型 @endif */
    uint8_t code_type;         /*!< @if Eng code type
                                    @else   包类型 @endif */
    uint8_t payload_type;      /*!< @if Eng payload type
                                    @else   发包模式 @endif */
    uint8_t power_level;        /*!< @if Eng power level,Scope:0~5
                                     @else   功率水平,取值范围:0~5 @endif */
    uint8_t tx_channel;        /*!< @if Eng tx channel
                                    @else   发送通道 @endif */
} br_rf_tx_start_t;

/**
 * @if Eng
 * @brief Enum of br radio frequency rx start
 * @else
 * @brief br开启射频长收参数
 * @endif
 */
typedef struct {
    uint16_t test_data_len;    /*!< @if Eng rx test data len,Scope:27~1021
                                    @else   接收测试数据长度,取值范围:27~1021 @endif */
    uint8_t addr[BD_ADDR_LEN]; /*!< @if Eng tx device address
                                    @else   对端设备地址 @endif */
    uint8_t rx_freq;           /*!< @if Eng rx frequency,Scope:0x00~0x4E,2402+x*2
                                    @else   接收频率,范围:0x00~0x4E,2402+x*2 @endif */
    uint8_t module_type;       /*!< @if Eng modulation type
                                    @else   调制类型 @endif */
    uint8_t code_type;         /*!< @if Eng code type
                                    @else   包类型 @endif */
    uint8_t rx_channel;        /*!< @if Eng rx channel
                                    @else   接收通道 @endif */
} br_rf_rx_start_t;

/**
 * @if Eng
 * @brief Enum of br radio frequency tx and rx end complete event
 * @else
 * @brief br关闭射频收发回调返回参数
 * @endif
 */
typedef struct {
    errcode_t status;           /*!< @if Eng event complete status
                                     @else   事件完成状态 @endif */
    uint16_t num_packets;       /*!< @if Eng num packets
                                     @else   数据包数 @endif */
    uint8_t rssi;               /*!< @if Eng rssi,Default:-127
                                     @else   接收信号强度指示,默认-127 @endif */
} br_rf_trx_end_cmp_evt_t;

/**
 * @if Eng
 * @brief Callback invoked when start radio frequenct tx.
 * @par Callback invoked when start radio frequenct tx.
 * @attention 1.This function is called in BTH service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the BTH service automatically.
 * @param  [in]  status error code.
 * @par Dependency:
 * @li  bts_def.h
 * @see br_factory_callbacks_t
 * @else
 * @brief  设置射频长发时的回调函数。
 * @par    设置射频长发时的回调函数。
 * @attention  1. 该回调函数运行于BTH service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由BTH service申请内存，也由BTH service释放，回调中不应释放。
 * @param  [in]  status 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @see br_factory_callbacks_t
 * @endif
 */
typedef void (*br_factory_rf_tx_start_callback)(errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when start radio frequenct rx.
 * @par Callback invoked when start radio frequenct rx.
 * @attention 1.This function is called in BTH service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the BTH service automatically.
 * @param  [in]  status error code.
 * @par Dependency:
 * @li  bts_def.h
 * @see br_factory_callbacks_t
 * @else
 * @brief  设置射频长收时的回调函数。
 * @par    设置射频长收时的回调函数。
 * @attention  1. 该回调函数运行于BTH service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由BTH service申请内存，也由BTH service释放，回调中不应释放。
 * @param  [in]  status 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @see br_factory_callbacks_t
 * @endif
 */
typedef void (*br_factory_rf_rx_start_callback)(errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when end radio frequenct tx and rx.
 * @par Callback invoked when end radio frequenct tx and rx.
 * @attention 1.This function is called in BTH service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the BTH service automatically.
 * @param  [in]  br_rf_trx_end_cmp_evt_t result struct.
 * @par Dependency:
 * @li  bts_def.h
 * @see br_factory_callbacks_t
 * @else
 * @brief  关闭射频收发时的回调函数。
 * @par    关闭射频收发时的回调函数。
 * @attention  1. 该回调函数运行于BTH service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由BTH service申请内存，也由BTH service释放，回调中不应释放。
 * @param  [in]  br_rf_trx_end_cmp_evt_t 执行结果返回结构体。。
 * @par 依赖:
 * @li  bts_def.h
 * @see br_factory_callbacks_t
 * @endif
 */
typedef void (*br_factory_rf_trx_end_callback)(br_rf_trx_end_cmp_evt_t *cmp_evt);

/**
 * @if Eng
 * @brief Callback invoked when reset radio frequenct tx and rx.
 * @par Callback invoked when reset radio frequenct tx and rx.
 * @attention 1.This function is called in BTH service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the BTH service automatically.
 * @param  [in]  status error code.
 * @par Dependency:
 * @li  bts_def.h
 * @see br_factory_callbacks_t
 * @else
 * @brief  重置射频收发时的回调函数。
 * @par    重置射频收发时的回调函数。
 * @attention  1. 该回调函数运行于BTH service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由BTH service申请内存，也由BTH service释放，回调中不应释放。
 * @param  [in]  status 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @see br_factory_callbacks_t
 * @endif
 */
typedef void (*br_factory_rf_reset_callback)(errcode_t status);

/**
 * @if Eng
 * @brief Struct of BR radio referency callback function.
 * @else
 * @brief BR factory 管理回调函数接口定义。
 * @endif
 */
typedef struct {
    br_factory_rf_tx_start_callback br_rf_tx_start_cb;         /*!< @if Eng radio frequency tx start
                                                                              callback.
                                                                      @else   射频长发回调函数。 @endif */
    br_factory_rf_rx_start_callback br_rf_rx_start_cb;         /*!< @if Eng radio frequency rx start
                                                                              callback.
                                                                      @else   射频长收回调函数。 @endif */
    br_factory_rf_trx_end_callback br_rf_trx_end_cb;           /*!< @if Eng radio frequency tx and rx end
                                                                              callback.
                                                                      @else   射频收发结束回调函数。 @endif */
    br_factory_rf_reset_callback br_rf_reset_cb;               /*!< @if Eng radio frequency tx and rx reset
                                                                              callback.
                                                                      @else   射频收发复位回调。 @endif */
} br_factory_callbacks_t;

/**
 * @if Eng
 * @brief  Register BR factory manager callbacks.
 * @par Description: Register BR factory manager callbacks.
 * @param  [in]  func Callback function.
 * @retval error code.
 * @else
 * @brief  注册 BR factory 管理回调函数。
 * @par Description: 注册 BR factory 管理回调函数。
 * @param  [in]  func 回调函数。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t br_factory_register_callbacks(br_factory_callbacks_t *func);

/**
 * @if Eng
 * @brief  start radio frequenct tx.
 * @par Description: start radio frequenct tx.
 * @param  [in]  param
 * @retval error code, read result will be returned at { @ref br_factory_rf_tx_start_callback }.
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  开启射频长发。
 * @par Description: 开启射频长发。
 * @param  [in]  param
 * @retval 执行结果错误码，读取结果将在{ @ref br_factory_rf_tx_start_callback }中返回。
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t br_factory_rf_tx_start(br_rf_tx_start_t *param);

/**
 * @if Eng
 * @brief  start radio frequenct rx.
 * @par Description: start radio frequenct rx.
 * @param  [in]  param
 * @retval error code, read result will be returned at { @ref br_factory_rf_rx_start_callback }.
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  开启射频长发。
 * @par Description: 开启射频长收。
 * @param  [in]  param
 * @retval 执行结果错误码，读取结果将在{ @ref br_factory_rf_rx_start_callback }中返回。
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t br_factory_rf_rx_start(br_rf_rx_start_t *param);

/**
 * @if Eng
 * @brief  end radio frequenct tx and rx.
 * @par Description: end radio frequenct tx and rx.
 * @retval error code, read result will be returned at { @ref br_factory_rf_trx_end_callback }.
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  关闭射频收发。
 * @par Description: 关闭射频收发。
 * @retval 执行结果错误码，读取结果将在{ @ref br_factory_rf_trx_end_callback }中返回。
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t br_factory_rf_trx_end(void);

/**
 * @if Eng
 * @brief  reset radio frequenct tx and rx.
 * @par Description: reset radio frequenct tx and rx.
 * @retval error code, read result will be returned at { @ref br_factory_rf_reset_callback }.
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  重置射频收发。
 * @par Description: 重置射频收发。
 * @retval 执行结果错误码，读取结果将在{ @ref br_factory_rf_reset_callback }中返回。
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t br_factory_rf_reset(void);

#endif