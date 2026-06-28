/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides HAL can-fd \n
 *
 * History: \n
 * 2023-02-22, Create file. \n
 */
#ifndef HAL_CAN_H
#define HAL_CAN_H

#include <stdint.h>
#include "common_def.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_can_api CAN
 * @ingroup  drivers_hal_can
 * @{
 */

#define HAL_DATA_BUFF_LENGTH   64

typedef enum hal_filter_type {
    HAL_RANGE_FILTER = 0x0,
    HAL_DUAL_ID_FILTER,
    HAL_CLASSIC_FILTER = 0x3
} hal_filter_type_t;

typedef enum hal_can_mode {
    HAL_NORMAL_OPREATION = 0x0,
    HAL_LOOP_BACK_MODE = 0x1
} hal_can_mode_t;

typedef enum hal_can_int_sts {
    HAL_CAN_FRAME_RECEIVED_INT,
    HAL_CAN_ERROR_PASSIVE_INT,
    HAL_CAN_BUS_OFF_INT,
} hal_can_int_sts_t;

typedef enum hal_data_length_code {
    HAL_BYTE_0 = 0x0,
    HAL_BYTE_1,
    HAL_BYTE_2,
    HAL_BYTE_3,
    HAL_BYTE_4,
    HAL_BYTE_5,
    HAL_BYTE_6,
    HAL_BYTE_7,
    HAL_BYTE_8,
    HAL_BYTE_12,
    HAL_BYTE_16,
    HAL_BYTE_20,
    HAL_BYTE_24,
    HAL_BYTE_32,
    HAL_BYTE_48,
    HAL_BYTE_64 = 0xF,
} hal_data_length_code_t;
/**
 * @if Eng
 * @brief  Norminal and Data bit time config.
 * @else
 * @brief  配置标称位时间和数据段位时间。
 * @endif
 */
typedef struct hal_can_global_config {
    hal_can_mode_t mode;
    uint32_t nseg1;
    uint32_t nseg2;
    uint32_t dseg1;
    uint32_t dseg2;
} hal_can_global_config_t;

/**
 * @if Eng
 * @brief  CAN filter structure.
 * @else
 * @brief  CAN滤波结构。
 * @endif
 */
typedef struct hal_can_filter {
    uint32_t fid1;                     /*!< @if Eng First ID of the filter.
                                            @else   滤波器的首要ID @endif */
    uint32_t fid2;                     /*!< @if Eng Second ID of the filter, which can be used as a mask,
                                                    secondary identifier, or right boundary.
                                            @else   滤波器的次要ID，可作为掩码、次要标识符或右边界。 @endif */
    uint8_t fifo_id;                   /*!< @if Eng Index of the Rx-FIFO bound to the filter.
                                                    Interrupt priorities are sorted by index in ASC order.
                                            @else   滤波器绑定的Rx-FIFO索引。中断优先级按索引升序排列。 @endif */
    uint8_t filter_type;               /*!< @if Eng Filter type.
                                            @else   滤波器模式 @endif */
} hal_can_filter_t;

typedef struct hal_can_filter_list {
    hal_can_filter_t *filters;
    uint8_t filter_num;
} hal_can_filter_list_t;

/**
 * @if Eng
 * @brief  CAN frame structure.
 * @else
 * @brief  CAN帧结构。
 * @endif
 */
typedef struct hal_can_msg {
    uint32_t std_id;
    uint32_t extend_id;
    uint8_t rtr;
    uint8_t ide;
    uint8_t fdf;
    uint8_t brs;
} hal_can_msg_t;

/**
 * @if Eng
 * @brief  Receive interrupt callback function.
 * @param  [in]  received_msg Received frame.
 * @else
 * @brief  接收中断回调函数。
 * @param  [in]  received_msg 接收到的帧结构。
 * @endif
 */
typedef void (*hal_can_int_callback_t)(uint8_t int_sts,
                                       hal_can_msg_t *received_msg, uint8_t *data, uint32_t length);

/**
 * @if Eng
 * @brief  Initialize CAN-FD.
 * @param  [in]  global_config CAN-FD global configuration, including bit time and operating mode configuration.
 * @param  [in]  std_filter Filtering configuration of the CAN-FD standard identifier.
 * @param  [in]  ex_filter Filtering configuration of the CAN-FD extended identifier.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  CAN-FD 初始化。
 * @param  [in]  global_config CAN-FD全局配置，包含位时间和工作模式配置。
 * @param  [in]  std_filter CAN-FD 标准标识符滤波配置。
 * @param  [in]  ex_filter CAN-FD 扩展标识符滤波配置。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_can_init(hal_can_global_config_t *global_config,
                       hal_can_filter_list_t *std_filter, hal_can_filter_list_t *ex_filter);

/**
 * @if Eng
 * @brief  CAN-FD deinitialize.
 * @param  [in]  bus CAN bus line select.
 * @else
 * @brief  CAN-FD 去初始化操作。
 * @param  [in]  bus CAN总线选择。
 * @endif
 */
void hal_can_deinit(void);

/**
 * @if Eng
 * @brief  Get Data or Remote frame from rx fifo.
 * @param  [in]  fifo_index Index of the Rx FIFO.
 * @param  [out] receive_msg CAN frame attribute got from the FIFO.
 * @param  [out] data CAN frame data.
 * @param  [out] length CAN frame data length.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从Rx FIFO中获取FIFO中所有报文。
 * @param  [in]  fifo_index Rx FIFO索引。
 * @param  [out] receive_msg 获取到的CAN报文属性。
 * @param  [out] data CAN报文数据。
 * @param  [out] length CAN报文数据长度。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_can_receive_msg(uint32_t fifo_index, hal_can_msg_t *receive_msg,
                              uint8_t *data, uint32_t *length);

/**
 * @if Eng
 * @brief  Add data or remote frame to Tx buffer.
 * @param  [in]  can_msg Specifies the attributes of the CAN frame to be sent.
 * @param  [in]  data CAN frame data.
 * @param  [in]  length CAN frame data length.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  向Tx buffer中加入数据并拉起发送请求。
 * @param  [in]  can_msg 发送的CAN报文属性。
 * @param  [in]  data CAN报文数据。
 * @param  [in]  length CAN报文数据长度。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_can_send_msg(hal_can_msg_t *can_msg, uint8_t *data, uint32_t length);

/**
 * @if Eng
 * @brief  Register a callback asociated with a CAN-FD interrupt cause.
 * @param  [in]  callback  Interrupt callback function.
 * @else
 * @brief  注册与CAN-FD中断原因关联的回调。
 * @param  [in]  callback  中断回调函数。
 * @endif
 */
void hal_can_register_int_callback(hal_can_int_callback_t callback);

/**
 * @if Eng
 * @brief  Unregister a callback asociated with a CAN-FD interrupt cause.
 * @else
 * @brief  解注册与CAN-FD中断原因关联的回调。
 * @endif
 */
void hal_can_unregister_int_callback(void);

/**
 * @if Eng
 * @brief  Register a callback asociated with a CAN-FD interrupt cause.
 * @retval For details,see @ref can_controller_sts_t.
 * @else
 * @brief  注册与CAN-FD中断原因关联的回调。
 * @retval 参考 @ref can_controller_sts_t 。
 * @endif
 */
uint32_t hal_can_get_controller_sts(void);

/**
 * @if Eng
 * @brief  Query the Transmit error count of the CAN controller.
 * @retval Transmit error count of the CAN controller.
 * @else
 * @brief  查询CAN控制器发送错误计数。
 * @retval CAN控制器发送错误计数。
 * @endif
 */
uint8_t hal_can_get_transmit_error_count(void);

/**
 * @if Eng
 * @brief  Query the Receive error count of the CAN controller.
 * @retval Receive error count of the CAN controller.
 * @else
 * @brief  查询CAN控制器接收错误计数。
 * @retval CAN控制器接收错误计数。
 * @endif
 */
uint8_t hal_can_get_receive_error_count(void);

/**
 * @if Eng
 * @brief  Set the base address of CAN-FD registers and SRAM.
 * @param  [in]  bus CAN bus line select.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置CAN-FD寄存器的基地址和SRAM基地址。
 * @param  [in]  bus CAN总线选择。
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_can_regs_init(void);

/**
 * @if Eng
 * @brief  Clear the base address of registers has been set by @ref hal_can_regs_init.
 * @param  [in]  bus CAN bus line select.
 * @else
 * @brief  清除由 @ref hal_can_regs_init 设置的基地址。
 * @param  [in]  bus CAN总线选择。
 * @endif
 */
void hal_can_regs_deinit(void);

/**
 * @if Eng
 * @brief  Get data length code of CAN frame data.
 * @param  [in] real_length The real length of the frame data.
 * @retval The data length code of the frame data.
 * @else
 * @brief  获取CAN报文数据的长度码
 * @param  [in] real_length 报文数据的真实长度。
 * @retval 报文数据的长度码
 * @endif
 */
uint8_t hal_can_get_dlc(uint32_t real_length);

/**
 * @if Eng
 * @brief  Get real length of CAN frame data.
 * @param  [in] dlc The data length code of CAN frame data.
 * @retval The real length of CAN frame data.
 * @else
 * @brief  获取报文数据的真实长度。
 * @param  [in] dlc 报文数据的长度码。
 * @retval 报文数据的真实长度。
 * @endif
 */
uint32_t hal_can_get_real_length(uint8_t dlc);

/**
 * @if Eng
 * @brief  Query the RX pin level of the CAN controller.
 * @param  [in]  bus CAN bus line select.
 * @retval 0x0 Low level.
 * @retval 0x1 High level.
 * @else
 * @brief  查询CAN控制器RX脚电平。
 * @param  [in]  bus CAN总线选择。
 * @retval 0x0 低电平。
 * @retval 0x1 高电平。
 * @endif
 */
uint8_t hal_can_get_rx_sts(void);

/**
 * @if Eng
 * @brief  Configuring Transmitter Delay Compensation Offset for CAN-FD
 * @param  [in]  enable Enable or disable TDC.
 * @param  [in]  offset TDC offset.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  配置CAN-FD的发送延迟补偿偏移
 * @param  [in]  enable 启用或禁用TDC。
 * @param  [in]  offset TDC偏移。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_can_fd_config_tdc(uint8_t enable, uint8_t offset);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif