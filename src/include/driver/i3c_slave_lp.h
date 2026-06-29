/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides I3C driver api \n
 *
 * History: \n
 * 2025-03-03, Create file. \n
 */
#ifndef I3C_SLAVE_LP_H
#define I3C_SLAVE_LP_H

#include "i3c_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief  i3c send completed callback to be called when tx data sending all completed.
           @ref uapi_i3c_slave_register_trx_callback is invoked.
 * @note   This callback is invoked in an interrupt context.
 * @param  tx_succ All data are sent successfully.

 * @else
 * @brief  i3c发送完成中断事件的回调函数，通过 @ref uapi_i3c_slave_register_trx_callback 注册到驱动中。
 * @note   这个函数是在中断上下文中执行的。
 * @param  tx_succ 所有数据是否正常发送。

 * @endif
 */
typedef void (*i3c_tx_done_cb_t)(uint32_t tx_succ);

/**
 * @if Eng
 * @brief  i3c receive callback to be called when rx data Received.
           @ref uapi_i3c_slave_register_trx_callback is invoked.
 * @note   This callback is invoked in an interrupt context.
 * @param  rx_succ All data are Receive successfully.
 * @param  puc_addr rx data addr.
 * @param  size rx data size.

 * @else
 * @brief  i3c接收完成中断事件的回调函数，通过 @ref uapi_i3c_slave_register_trx_callback 注册到驱动中。
 * @note   这个函数是在中断上下文中执行的。
 * @param  rx_succ 所有数据是否正常接收。
 * @param  puc_addr rx数据地址。
 * @param  size rx数据长度。

 * @endif
 */
typedef void (*i3c_rx_done_cb_t)(uint32_t rx_succ, uint8_t *puc_addr, uint32_t size);

/**
 * @if Eng
 * @brief  i3c receive start callback to be called when data received from the master.
           @ref uapi_i3c_slave_register_trx_callback is invoked.
 * @note   This callback is invoked in an interrupt context.

 * @else
 * @brief  i3c接收开始中断事件的回调函数，通过 @ref uapi_i3c_slave_register_trx_callback 注册到驱动中。
 * @note   这个函数是在中断上下文中执行的。

 * @endif
 */
typedef void (*i3c_rx_start_cb_t)(void);

/**
 * @if Eng
 * @brief  Initialize the I3C as slave according to the specified parameters.
 * @param  [in]  i3c_static_addr The slave address when the I3C is operating as a slave.
 * @param  [in]  mode The slave mode when the I3C is operating as a slave. For details, see @ref i3c_mode_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  根据指定的参数初始化该I3C为从机。
 * @param  [in]  i3c_static_addr I3C作为从机工作时的从机地址。
 * @param  [in]  mode I3C作为从机工作时的从机模式，参考 @ref i3c_mode_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败 参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i3c_slave_init(uint8_t i3c_static_addr, i3c_mode_t mode);

/**
 * @if Eng
 * @brief  Deinitialize the I3C as slave.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化I3C从机。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败 参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i3c_slave_deinit(void);

/**
 * @if Eng
 * @brief  register i3c tx and rx callback.
 * @param  [in]  pfun_tx_done_func  tx done callback.
 * @param  [in]  pfun_rx_done_func  rx done callback.
 * @param  [in]  pfun_rx_start_func  rx start callback.
 * @else
 * @brief  注册I3C tx和rx回调函数。
 * @param  [in]  pfun_tx_done_func tx完成回调函数。
 * @param  [in]  pfun_rx_done_func rx完成回调函数。
 * @param  [in]  pfun_rx_start_func rx开始回调函数。
 * @endif
 */
void uapi_i3c_slave_register_trx_callback(i3c_tx_done_cb_t pfun_tx_done_func, i3c_rx_done_cb_t pfun_rx_done_func,
                                          i3c_rx_start_cb_t pfun_rx_start_func);

/**
 * @if Eng
 * @brief  Get the configuration value corresponding to a single command.
 * @param  [in] cmd Command to be get.
 * @retval The configuration value corresponding to a single command.
 * @else
 * @brief  获取单个命令对应的配置值。
 * @param  [in] cmd 想要查询的命令。
 * @retval 单个命令对应的配置值。
 * @endif
 */
uint8_t uapi_i3c_slave_cmd_table_value_get(uint8_t cmd);

/**
 * @if Eng
 * @brief  Setting the configuration value of a single command.
 * @param  [in] cmd Command to be set.
 * @param  [in] value The configuration value corresponding to a single command.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置单个命令的配置值。
 * @param  [in] cmd 想要查询的命令。
 * @param  [in] value 单个命令对应的配置值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败 参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i3c_slave_cmd_table_value_set(uint8_t cmd, uint8_t value);

/**
 * @if Eng
 * @brief  Slave writes a buffer to Master.
 * @param  [in]  data The information pointer of send data.
 * @param  [in]  size The size of send data.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从机将数据发送给主机。
 * @param  [in]  data 发送数据的信息指针。
 * @param  [in]  size 发送数据的大小。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i3c_slave_write(uint8_t *data, uint16_t data_len);

/**
 * @if Eng
 * @brief  Slave hardsend enable.
 * @param  [in]  enable Enable or disable.
 * @param  [in]  cpu_type CPU type. For details, see @ref hal_i3c_cpu_type_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  i3c硬发使能开关。
 * @param  [in]  enable 是否使能。
 * @param  [in]  cpu_type 使能的cpu。参考 @ref hal_i3c_cpu_type_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i3c_slave_hardsend_enable(bool enable, uint8_t cpu_type);

/**
 * @if Eng
 * @brief  Slave hardsend data config.
 * @param  [in]  base_addr The information address of hardsend data.
 * @param  [in]  length The size of hardsend data.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从机硬发数据的配置。
 * @param  [in]  base_addr 硬发数据的信息地址。
 * @param  [in]  length 硬发数据的大小。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i3c_slave_hardsend_config(uint32_t base_addr, uint8_t length);

/**
 * @if Eng
 * @brief  Get i3c slave hardsend time.
 * @param  [in]  cpu_type The type of i3c cpu type. For details, see @ref hal_i3c_cpu_type_t.
 * @param  [in]  on_time I3C slave hardsend time of on type.
 * @param  [in]  off_time I3C slave hardsend time of off type.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从机硬发数据的配置。
 * @param  [in]  cpu_type i3c的cpu类型。参考 @ref hal_i3c_cpu_type_t 。
 * @param  [in]  on_time on类型硬发次数。
 * @param  [in]  off_time off类型硬发次数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i3c_slave_hardsend_time_get(uint8_t cpu_type, uint8_t *on_time, uint8_t *off_time);

/**
 * @if Eng
 * @brief  Slave receives a buffer form Master.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从机从主机读取数据。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i3c_slave_read(void);

/**
 * @if Eng
 * @brief  Slave hardsend enable.
 * @param  [in]  enable Enable or disable.
 * @param  [in]  cpu_type CPU type. For details, see @ref hal_i3c_cpu_type_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  i3c转发使能开关。
 * @param  [in]  enable 是否使能。
 * @param  [in]  cpu_type 使能的cpu。参考 @ref hal_i3c_cpu_type_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i3c_slave_msg_forward_enable(bool enable, uint8_t cpu_type);

/**
 * @if Eng
 * @brief  Reset slave receiving FIFO.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  重置RX FIFO。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i3c_rx_fifo_reset(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
