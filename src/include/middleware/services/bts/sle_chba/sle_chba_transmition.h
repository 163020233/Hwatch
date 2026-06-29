/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 *
 * Description: SLE CHBA NET DEVICE PUBLIC API module.
 */
#ifndef SLE_CHBA_TRANSMITION_H
#define SLE_CHBA_TRANSMITION_H

#include "sle_common.h"
#include "errcode.h"

typedef struct {
    uint8_t conn_id;           /*!< @if Eng connection ID
                                    @else   连接ID @endif */
    sle_addr_t remote_addr;    /*!< @if Eng remote address
                                    @else   远端设备地址 @endif */
    uint32_t tx_pkts_cnt;      /*!< @if Eng number of sent packets
                                    @else   已发送报文数量 @endif */
    uint32_t tx_bytes;         /*!< @if Eng number of bytes sent
                                    @else   已发送字节数 @endif */
    uint32_t rx_pkts_cnt;      /*!< @if Eng number of received packets
                                    @else   已接收报文数量 @endif */
    uint32_t rx_bytes;         /*!< @if Eng received bytes number
                                    @else   已接收字节数 @endif */
} sle_ip_link_info;

/**
 * @if Eng
 * @brief  Get link information.
 * @par Description: Get link information.
 * @param  [in]  conn_id connection ID.
 * @param  [in]  link Pointer to the linkinfo to be obtained.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  获取链路信息。
 * @par Description: 获取链路信息。
 * @param  [in] conn_id 连接 ID。
 * @param  [in] link 待获取的linkinfo指针 { @ref sle_ip_link_info }。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_chba_netdev_get_linkinfo(uint16_t conn_id, sle_ip_link_info *link);

/**
 * @if Eng
 * @brief  Send data.
 * @par Description: Send data.
 * @param  [in]  data sent data.
 * @param  [in]  len data length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  发送数据。
 * @par Description: 发送数据。
 * @param  [in] data 发送的数据。
 * @param  [in] len 数据长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t sle_chba_netdev_driver_send(uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief Callback invoked when sle chba is busy.
 * @par Callback invoked when sle chba is busy.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  sle chba驱动缓存满的回调函数。
 * @par    sle chba驱动缓存满的回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*sle_chba_netdev_stop_queue_callback)(void);

/**
 * @if Eng
 * @brief Callback invoked when sle chba is idle.
 * @par Callback invoked when sle chba is idle.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  sle chba驱动空闲的回调函数。
 * @par    sle chba驱动空闲的回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*sle_chba_netdev_wake_queue_callback)(void);

/**
 * @if Eng
 * @brief Callback invoked when sle chba driver is enabled.
 * @par Callback invoked when sle chba driver is enabled.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  sle chba驱动开启的回调函数。
 * @par    sle chba驱动开启的回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*sle_chba_netdev_set_link_up_callback)(void);

/**
 * @if Eng
 * @brief Callback invoked when sle chba driver is disabled.
 * @par Callback invoked when sle chba driver is disabled.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  sle chba驱动关闭的回调函数。
 * @par    sle chba驱动关闭的回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*sle_chba_netdev_set_link_down_callback)(void);

/**
 * @if Eng
 * @brief Callback invoked when receiving the package.
 * @par Callback invoked when receiving the package.
 * @param [in] data received data.
 * @param [in] len  data length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  收包的回调函数。
 * @par    收包的回调函数。
 * @param [in] data 收到的数据.
 * @param [in] len  数据长度.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*sle_chba_netdev_input_callback)(uint8_t *data, uint16_t len);

typedef struct {
    sle_chba_netdev_stop_queue_callback stop_queue_cb;              /*!< @if Eng Chba net device stop queue callback.
                                                                    @else   CHBA网络设备停止发送队列回调函数。 @endif */
    sle_chba_netdev_wake_queue_callback wake_queue_cb;              /*!< @if Eng Chba net device wake queue callback.
                                                                    @else   CHBA网络设备唤醒发送队列回调函数。 @endif */
    sle_chba_netdev_set_link_up_callback set_link_up_cb;            /*!< @if Eng Chba net device set link up callback.
                                                                    @else   CHBA网络设备设置链路启用回调函数。 @endif */
    sle_chba_netdev_set_link_down_callback set_link_down_cb;        /*!< @if Eng Chba net device set link down callback.
                                                                    @else   CHBA网络设备设置链路禁用回调函数。 @endif */
    sle_chba_netdev_input_callback netdev_input_cb;                 /*!< @if Eng Chba net device input callback.
                                                                    @else   CHBA网络设备数据上报回调函数。 @endif */
} sle_chba_netdev_callbacks_t;

/**
 * @if Eng
 * @brief  Register sle chba callbacks.
 * @par Description: Register sle chba callbacks.
 * @param  [in] func callback function.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册 sle chba 回调函数。
 * @par Description: 注册回调函数。
 * @param  [in] func 回调函数
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t sle_chba_netdev_register_callbacks(sle_chba_netdev_callbacks_t *func);
#endif