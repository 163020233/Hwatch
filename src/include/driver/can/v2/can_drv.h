/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provides autosar can interface. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-04-05, Create file. \n
 */
#ifndef CAN_DRV_H
#define CAN_DRV_H

#if defined (CONFIG_CAN_USE_V2_DRIVER)
#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "common_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_can_autosar CAN
 * @ingroup drivers_driver
 * @{
 */

#define CAN_EXTENDED_MARK_MASK 0x80000000
#define CAN_FD_MARK_MASK 0x40000000
#define CAN_ID_MASK 0x3FFFFFFF

typedef enum {
    CAN_NORMAL_OPREATION = 0x0,
    CAN_LOOPBACK_MODE = 0x1
} can_opreation_mode_t;

typedef enum {
    CAN_RX_STORAGE_RX_FIFO_0 = 0xFE,
    CAN_RX_STORAGE_RX_FIFO_1 = 0xFF
} can_rx_fifo_storage_t;

typedef enum {
    CAN_CS_UNINIT = 0,    /* 未初始化状态 */
    CAN_CS_STARTED,       /* 已启动状态 */
    CAN_CS_STOPPED,       /* 已停止状态 */
    CAN_CS_SLEEP,          /* 休眠状态 */
    CAN_CS_MAX
} can_controller_state_type_t;

typedef enum {
    CAN_ERRORSTATE_ACTIVE = 0,    /* 主动错误状态 */
    CAN_ERRORSTATE_PASSIVE,       /* 被动错误状态 */
    CAN_ERRORSTATE_BUSOFF         /* 总线关闭状态 */
} can_error_state_t;

typedef enum {
    CAN_INTERRUPT_MODE = 0,   /* 中断模式 */
    CAN_MIXED_MODE,       /* 混合模式，仅发送支持，其余场景配置该值表现与中断模式一致 */
    CAN_POLLING_MODE      /* 轮询模式 */
} can_processing_mode_t;

typedef enum {
    CAN_STANDARD_FILTER = 0,
    CAN_EXTENDED_FILTER
} can_filter_id_type_t;

typedef enum {
    CAN_RANGE_FILTER = 0x0, /* 双边界模式 */
    CAN_DUAL_ID_FILTER,     /* 双ID模式 */
    CAN_CLASSIC_FILTER      /* ID & MASK模式 */
} can_filter_mode_t;


typedef struct {
    uint32_t id;           /* CAN ID */
    uint8_t ide;           /* Extend ID标识 */
    uint8_t fdf;           /* FD frame标识 */
    uint8_t rtr;           /* 远程帧标识 */
    uint8_t length;        /* 数据长度 */
    uint8_t *data;         /* 数据指针 */
} can_frame_t;

/*
    位时间分配:
    默认包含1tq同步段，不视为seg1和seg2的一部分，采样点视为seg1与seg2之间的边沿
    seg1 = prop_seg + phase_seg1 - 1;
    seg2 = phase_seg2 - 1;
    baudrate = cclk_freq / ((seg1 + seg2 + 3) * (brp + 1))
*/

typedef struct {
    uint8_t can_fd_brp;         /* CAN FD 数据段波特率预分频器 */
    uint8_t can_fd_seg1;        /* CAN FD 数据段时间段1的长度 */
    uint8_t can_fd_seg2;        /* CAN FD 数据段时间段2的长度 */
    uint8_t can_fd_sjw;         /* CAN FD 数据段同步跳跃宽度 */
    uint8_t can_fd_ssp_offset;  /* CAN FD 第二采样点偏移 */
    uint8_t can_fd_tx_brs;      /* 传输速率切换标志 */
    uint32_t fd_baudrate;       /* CAN FD 波特率 */
} can_fd_baudrate_config_t;

typedef struct {
    uint8_t can_brp;           /* 波特率预分频器 */
    uint8_t can_seg1;          /* 时间段1的长度 */
    uint8_t can_seg2;          /* 时间段2的长度 */
    uint8_t can_sjw;           /* 同步跳跃宽度 */
    uint32_t baudrate;         /* 波特率 */
    can_fd_baudrate_config_t fd_baudrate_config; /* FD波特率配置 */
} can_baudrate_config_t;

typedef struct {
    can_processing_mode_t busoff_processing; /* 总线关闭处理模式 */
    can_processing_mode_t rx_buffer_processing;     /* 接收buffer处理模式 */
    can_processing_mode_t rx_fifo0_processing;      /* 接收FIFO0处理模式 */
    can_processing_mode_t rx_fifo1_processing;      /* 接收FIFO1处理模式 */
    can_processing_mode_t tx_processing;     /* 发送处理模式 */
    can_processing_mode_t wakeup_processing; /* 唤醒处理模式 */
} can_processing_t;

typedef struct {
    can_processing_t processing_config; /* 控制器处理模式配置 */
    can_baudrate_config_t *baudrate_config;  /* 波特率配置指针 */
    can_opreation_mode_t opreation_mode;     /* 控制器工作模式 */
} can_controller_config_t;

typedef struct {
    can_filter_id_type_t filter_id_type; /* 过滤器ID类型 */
    can_filter_mode_t filter_type;       /* 过滤器掩码类型 */
    uint32_t fid1;                       /* 过滤器掩码1 */
    uint32_t fid2;                       /* 过滤器掩码2 */
} can_hw_filter_t;

typedef struct {
    uint8_t filter_index;              /* 过滤器索引，用于标识过滤器 */
    uint8_t storage_buffer_index;      /* 存储缓冲区索引，欲存储至FIFO参考@ref can_rx_fifo_storage_t */
    can_hw_filter_t *hw_filter;        /* 硬件过滤器，若存储至固定buffer，
                                          则接收与@ref can_hw_filter_t.fid1完全一致的帧 */
} can_filter_config_t;

typedef enum {
    CAN_TT_DISABLE = 0,
    CAN_TT_LEVEL1 = 1,
    CAN_TT_LEVEL2 = 2,
    CAN_TT_LEVEL0 = 3,
} tt_opreation_mode;

typedef enum {
    CAN_TT_TX_REF_TRIGGER = 0,
    CAN_TT_TX_REF_TRIGGER_GAP = 1,
    CAN_TT_TX_TRIGGER_SINGLE = 2,
    CAN_TT_TX_TRIGGER_CONTINOUS = 3,
    CAN_TT_TX_TRIGGER_ARBITRATION = 4,
    CAN_TT_TX_TRIGGER_MERGED = 5,
    CAN_TT_WATCH_TRIGGER = 6,
    CAN_TT_WATCH_TRIGGER_GAP = 7,
    CAN_TT_RX_TRIGGER = 8,
    CAN_TT_TIME_BASE_TRIGGER = 9,
    CAN_TT_END_OF_LIST = 10
} tt_trigger_type_t;

typedef struct {
    uint32_t tt_opreation_mode; /* TTCAN 工作模式 */
    uint32_t tt_gap_enable;     /* TTCAN GAP功能使能 */
    uint32_t is_time_master;    /* TTCAN time master模式使能 */
    uint32_t sync_deviation_limit; /* 同步跨度限制 */
    uint32_t init_ref_trigger_offset; /* 初始参考消息偏移 */
    uint32_t global_time_filter; /* 全局时间过滤 */
    uint32_t clock_calibration; /* 时间自动补偿 */
    uint32_t basic_cycle_num; /* 基本周期个数 */
    uint32_t tx_enable_window; /* tx发送窗口长度 */
    uint32_t tx_trigger_num; /* tx触发器数量 */
    uint32_t tur_numerator; /* TUR分子 */
    uint32_t tur_denominator; /* TUR分母 */
} can_tt_config_t;

typedef struct {
    uint32_t trigger_index; /* 触发器索引 */
    uint32_t time_mark;    /* 触发器触发时间戳 */
    uint32_t trigger_type; /* 触发器类型 */
    uint32_t start_cycle;  /* 触发器起始周期 */
    uint32_t cycle_period; /* 触发器触发频率 */
    uint32_t tx_index; /* 绑定的tx_buffer索引(Tx Trigger限定) */
    uint32_t filter_type; /* 绑定的rx_filter类型(Rx Trigger限定) */
    uint32_t filter_index; /* 绑定的rx_filter索引(Rx Trigger限定) */
} can_time_trigger_t;

/**
 * @if Eng
 * @brief  Initialize the CAN controller.
 * @param  [in]  controller_config Pointer to the CAN controller configuration structure.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化CAN控制器。
 * @param  [in]  controller_config 指向CAN控制器配置结构体的指针。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_controller_init(uint8_t controller_id, can_controller_config_t *controller_config);

/**
 * @if Eng
 * @brief  Deinitialize the CAN controller.
 * @param  [in]  controller_id CAN controller identifier.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  反初始化CAN控制器。
 * @param  [in]  controller_id CAN控制器标识符。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_controller_deinit(uint8_t controller_id);

/**
 * @if Eng
 * @brief  Set the CAN filter configuration.
 * @param  [in]  controller_id CAN controller identifier.
 * @param  [in]  filter_config Pointer to the CAN filter configuration structure.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置CAN过滤器配置。
 * @param  [in]  controller_id CAN控制器标识符。
 * @param  [in]  filter_config 指向CAN过滤器配置结构体的指针。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_controller_set_filter(uint8_t controller_id, can_filter_config_t *filter_config);

/**
 * @if Eng
 * @brief  Set the CAN controller baud rate.
 * @param  [in]  controller_id CAN controller identifier.
 * @param  [in]  baudrate_config CAN controller baud rate configuration.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置CAN控制器波特率。
 * @param  [in]  controller_id CAN控制器标识符。
 * @param  [in]  baudrate_config CAN控制器波特率配置。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_controller_set_baudrate(uint8_t controller_id, can_baudrate_config_t *baudrate_config);

/**
 * @if Eng
 * @brief  Set the CAN controller state.
 * @param  [in]  controller_id CAN controller identifier.
 * @param  [in]  state Controller state to set.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置CAN控制器状态。
 * @param  [in]  controller_id CAN控制器标识符。
 * @param  [in]  state 要设置的控制器状态。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_controller_set_state(uint8_t controller_id, can_controller_state_type_t state);

/**
 * @if Eng
 * @brief  Enable controller interrupts.
 * @param  [in]  controller_id CAN controller identifier.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  启用控制器中断。
 * @param  [in]  controller_id CAN控制器标识符。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_controller_enable_interrupts(uint8_t controller_id);

/**
 * @if Eng
 * @brief  Disable controller interrupts.
 * @param  [in]  controller_id CAN controller identifier.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  禁用控制器中断。
 * @param  [in]  controller_id CAN控制器标识符。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_controller_disable_interrupts(uint8_t controller_id);

/**
 * @if Eng
 * @brief  Get the error state of the CAN controller.
 * @param  [in]  controller_id CAN controller identifier.
 * @param  [out] error_state Pointer to store the error state.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取CAN控制器的错误状态。
 * @param  [in]  controller_id CAN控制器标识符。
 * @param  [out] error_state 存储错误状态的指针。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_controller_get_error_state(uint8_t controller_id, can_error_state_t *error_state);

/**
 * @if Eng
 * @brief  Get the operating state of the CAN controller.
 * @param  [in]  controller_id CAN controller identifier.
 * @param  [out] state Pointer to store the controller state.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取CAN控制器的工作状态。
 * @param  [in]  controller_id CAN控制器标识符。
 * @param  [out] state 存储控制器工作状态的指针。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_controller_get_state(uint8_t controller_id, can_controller_state_type_t *state);

/**
 * @if Eng
 * @brief  Get the receive error counter of the CAN controller.
 * @param  [in]  controller_id CAN controller identifier.
 * @param  [out] rx_error_counter Pointer to store the receive error counter.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取CAN控制器的接收错误计数器。
 * @param  [in]  controller_id CAN控制器标识符。
 * @param  [out] rx_error_counter 存储接收错误计数器的指针。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_controller_get_rx_error_counter(uint8_t controller_id, uint8_t *rx_error_counter);

/**
 * @if Eng
 * @brief  Get the transmit error counter of the CAN controller.
 * @param  [in]  controller_id CAN controller identifier.
 * @param  [out] tx_error_counter Pointer to store the transmit error counter.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取CAN控制器的发送错误计数器。
 * @param  [in]  controller_id CAN控制器标识符。
 * @param  [out] tx_error_counter 存储发送错误计数器的指针。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_controller_get_tx_error_counter(uint8_t controller_id, uint8_t *tx_error_counter);

/**
 * @if Eng
 * @brief  Send data using the CAN controller.
 * @param  [in]  controller_id CAN controller identifier.
 * @param  [in,out]  tx_buffer_index Buffer index for transmission. Direction depends on 'use_tx_fifo':
 *                  - If 'use_tx_fifo' is 'true', it is an **output** parameter that returns the allocated index.
 *                  - If 'use_tx_fifo' is 'false', it is an **input** parameter that specifies the buffer index to use.
 * @param  [in]  can_frame Pointer to the CAN frame data to be transmitted.
 * @param  [in]  use_tx_fifo Indicates whether to use the transmission FIFO:
 *                  - 'true': Use FIFO, 'tx_buffer_index' is output.
 *                  - 'false': Specify buffer index, 'tx_buffer_index' is input.
 * @param  [in]  is_polling Indicates whether to perform a polling operation.
                    - If `true`, the transmission completion will be reported when calling `process_write_polling`.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  使用CAN控制器发送数据。
 * @param  [in]  controller_id CAN控制器标识符。
 * @param  [in,out]  tx_buffer_index 传输使用的缓冲区索引。方向由 'use_tx_fifo' 决定：
 *                  - 如果 'use_tx_fifo' 为 'true'，则为 **输出** 参数，返回分配的索引。
 *                  - 如果 'use_tx_fifo' 为 'false'，则为 **输入** 参数，指定要使用的缓冲区索引。
 * @param  [in]  can_frame 指向要传输的CAN帧数据的指针。
 * @param  [in]  use_tx_fifo 表示是否使用传输FIFO：
 *                  - 'true': 使用FIFO，'tx_buffer_index' 为输出。
 *                  - 'false': 指定缓冲区索引，'tx_buffer_index' 为输入。
 * @param  [in]  is_polling 表示是否执行轮询操作，若为'true'，则在调用process_write_polling时上报发送完成。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_send(uint8_t controller_id, uint8_t *tx_buffer_index, can_frame_t *can_frame,
                        bool use_tx_fifo, bool is_polling);

/**
 * @if Eng
 * @brief  Cancel a pending CAN transmission.
 * @param  [in]  controller_id CAN controller identifier.
 * @param  [in]  tx_index Index of the buffer to cancel the transmission.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  取消一个待处理的CAN传输。
 * @param  [in]  controller_id CAN控制器标识符。
 * @param  [in]  tx_index 要取消传输的缓冲区索引。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_send_cancel(uint8_t controller_id, uint8_t tx_index);

/**
 * @if Eng
 * @brief  Query the RX pin level status of the CAN controller.
 * @param  [in]  controller_id CAN controller ID selection.
 * @param  [out] rx_sts Pointer to store the RX pin level status.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure, refer to @ref errcode_t.
 * @else
 * @brief  查询CAN控制器RX脚电平。
 * @param  [in]  controller_id CAN控制器标识符。
 * @param  [out] rx_sts 存储RX引脚电平状态的指针。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_controller_get_rx_sts(uint8_t controller_id, uint8_t *rx_sts);

/**
 * @if Eng
 * @brief  Transmission callback function template.
 * @param  [in]  controller_id The CAN controller identifier reporting the transmission callback function.
 * @param  [in]  buffer_index The index of the transmission hardware unit that has completed sending.
 * @else
 * @brief  发送回调函数模板。
 * @param  [in]  controller_id 上报发送回调函数的CAN控制器标识符。
 * @param  [in]  buffer_index 发送完成的发送硬件单元索引。
 * @endif
 */
typedef void (*can_tx_callback_t)(uint8_t controller_id, uint8_t buffer_index);

/**
 * @if Eng
 * @brief  Register the transmission callback function.
 * @param  [in]  callback The transmission callback function to register.
 * @note   -This function can only be called once.
           -Subsequent calls will have no effect until the callback is unregistered.
 * @else
 * @brief  注册发送回调函数。
 * @param  [in]  callback 要注册的发送回调函数。
 * @note   该函数只能调用一次。后续调用将无效，直到回调函数被注销。
 * @endif
 */
void uapi_can_register_tx_callback(can_tx_callback_t callback);

/**
 * @if Eng
 * @brief  Unregister the transmission callback function.
 * @else
 * @brief  注销发送回调函数。
 * @endif
 */
void uapi_can_unregister_tx_callback(void);

/**
 * @if Eng
 * @brief  Transmission cancellation callback function template.
 * @param  [in]  controller_id The CAN controller identifier reporting the transmission cancellation callback function.
 * @param  [in]  buffer_index The index of the transmission hardware unit that has been canceled.
 * @else
 * @brief  发送取消回调函数模板。
 * @param  [in]  controller_id 上报发送取消回调函数的CAN控制器标识符。
 * @param  [in]  buffer_index 被取消的发送硬件单元索引。
 * @endif
 */
typedef void (*can_tx_cancel_callback_t)(uint8_t controller_id, uint8_t buffer_index);

/**
 * @if Eng
 * @brief  Register the transmission cancellation callback function.
 * @param  [in]  callback The transmission cancellation callback function to register.
 * @note   - This function can only be called once.
 *         - Subsequent calls will have no effect until the callback is unregistered.
 * @else
 * @brief  注册发送取消回调函数。
 * @param  [in]  callback 要注册的发送取消回调函数。
 * @note   该函数只能调用一次。后续调用将无效，直到回调函数被注销。
 * @endif
 */
void uapi_can_register_tx_cancel_callback(can_tx_cancel_callback_t callback);

/**
 * @if Eng
 * @brief  Unregister the transmission cancellation callback function.
 * @else
 * @brief  注销发送取消回调函数。
 * @endif
 */
void uapi_can_unregister_tx_cancel_callback(void);

/**
 * @if Eng
 * @brief  Reception callback function template.
 * @param  [in]  controller_id The CAN controller identifier reporting the reception callback function.
 * @param  [in]  filter_index The index of the filter that matched the received message.
 * @param  [in]  can_frame Pointer to the received CAN frame data.
 * @else
 * @brief  接收回调函数模板。
 * @param  [in]  controller_id 上报接收回调函数的CAN控制器标识符。
 * @param  [in]  filter_index 匹配接收到的消息的过滤器索引。
 * @param  [in]  can_frame 接收到的CAN帧数据指针。
 * @endif
 */
typedef void (*can_rx_callback_t)(uint8_t controller_id, uint8_t filter_index, can_frame_t *can_frame);

/**
 * @if Eng
 * @brief  Register the reception callback function.
 * @param  [in]  callback The reception callback function to register.
 * @note   - This function can only be called once.
           - Subsequent calls will have no effect until the callback is unregistered.
 * @else
 * @brief  注册接收回调函数。
 * @param  [in]  callback 要注册的接收回调函数。
 * @note   该函数只能调用一次。后续调用将无效，直到回调函数被注销。
 * @endif
 */
void uapi_can_register_rx_callback(can_rx_callback_t callback);

/**
 * @if Eng
 * @brief  Unregister the reception callback function.
 * @else
 * @brief  注销接收回调函数。
 * @endif
 */
void uapi_can_unregister_rx_callback(void);


/**
 * @if Eng
 * @brief  Bus off callback function template.
 * @param  [in]  controller_id The CAN controller identifier reporting the bus off callback function.
 * @else
 * @brief  总线关闭回调函数模板。
 * @param  [in]  controller_id 上报总线关闭回调函数的CAN控制器标识符。
 * @endif
 */
typedef void (*can_bus_off_callback_t)(uint8_t controller_id);

/**
 * @if Eng
 * @brief  Register the bus-off callback function.
 * @param  [in]  callback The bus-off callback function to register.
 * @note   - This function can only be called once.
           - Subsequent calls will have no effect until the callback is unregistered.
 * @else
 * @brief  注册总线关闭回调函数。
 * @param  [in]  callback 要注册的总线关闭回调函数。
 * @note   该函数只能调用一次。后续调用将无效，直到回调函数被注销。
 * @endif
 */
void uapi_can_register_bus_off_callback(can_bus_off_callback_t callback);

/**
 * @if Eng
 * @brief  Unregister the bus-off callback function.
 * @else
 * @brief  注销总线关闭回调函数。
 * @endif
 */
void uapi_can_unregister_bus_off_callback(void);

/**
 * @if Eng
 * @brief  Wakeup callback function template.
 * @param  [in]  controller_id The CAN controller identifier reporting the wakeup callback function.
 * @else
 * @brief  唤醒回调函数模板。
 * @param  [in]  controller_id 上报唤醒回调函数的CAN控制器标识符。
 * @endif
 */
typedef void (*can_wakeup_callback_t)(uint8_t controller_id);

/**
 * @if Eng
 * @brief  Register the wake-up callback function.
 * @param  [in]  callback The wake-up callback function to register.
 * @note   - This function can only be called once.
           - Subsequent calls will have no effect until the callback is unregistered.
 * @else
 * @brief  注册唤醒回调函数。
 * @param  [in]  callback 要注册的唤醒回调函数。
 * @note   该函数只能调用一次。后续调用将无效，直到回调函数被注销。
 * @endif
 */
void uapi_can_register_wakeup_callback(can_wakeup_callback_t callback);

/**
 * @if Eng
 * @brief  Unregister the wake-up callback function.
 * @else
 * @brief  注销唤醒回调函数。
 * @endif
 */
void uapi_can_unregister_wakeup_callback(void);

/**
 * @if Eng
 * @brief  Controller state change callback function template.
 * @param  [in]  controller_id The CAN controller identifier reporting the state change callback function.
 * @param  [in]  target_state The target state of the CAN controller.
 * @else
 * @brief  控制器状态变化回调函数模板。
 * @param  [in]  controller_id 上报控制器状态变化回调函数的CAN控制器标识符。
 * @param  [in]  target_state CAN控制器的目标状态。
 * @endif
 */
typedef void (*can_state_callback_t)(uint8_t controller_id, can_controller_state_type_t target_state);

/**
 * @if Eng
 * @brief  Register the state transition callback function.
 * @param  [in]  callback The state transition callback function to register.
 * @note   - This function can only be called once.
           - Subsequent calls will have no effect until the callback is unregistered.
 * @else
 * @brief  注册状态转移回调函数。
 * @param  [in]  callback 要注册的状态转移回调函数。
 * @note   该函数只能调用一次。后续调用将无效，直到回调函数被注销。
 * @endif
 */
void uapi_can_register_state_callback(can_state_callback_t callback);

/**
 * @if Eng
 * @brief  Unregister the state transition callback function.
 * @else
 * @brief  注销状态转移回调函数。
 * @endif
 */
void uapi_can_unregister_state_callback(void);

/**
 * @if Eng
 * @brief  Main function for CAN write operations.
 * @else
 * @brief  CAN写操作主函数。
 * @endif
 */
void uapi_can_process_write_polling(void);

/**
 * @if Eng
 * @brief  Main function for CAN read operations.
 * @else
 * @brief  CAN读操作主函数。
 * @endif
 */
void uapi_can_process_read_polling(void);

/**
 * @if Eng
 * @brief  Main function for handling bus-off state.
 * @else
 * @brief  处理总线关闭状态的主函数。
 * @endif
 */
void uapi_can_process_bus_off_polling(void);

/**
 * @if Eng
 * @brief  Main function for wake-up operations.
 * @else
 * @brief  唤醒操作主函数。
 * @endif
 */
void uapi_can_process_wakeup_polling(void);

#if defined (CONFIG_CAN_SUPPORT_TTCAN)
/**
 * @if Eng
 * @brief  Configure the TTCAN basic settings.
 * @param  [in]  controller_id CAN controller identifier.
 * @param  [in]  tt_config TTCAN basic configuration.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  TTCAN 基础配置
 * @param  [in]  controller_id CAN控制器标识符。
 * @param  [in]  tt_config TTCAN基本配置项
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_tt_config(uint8_t controller_id, can_tt_config_t *tt_config);

/**
 * @if Eng
 * @brief  Configure TTCAN reference message settings.
 * @param  [in]  controller_id CAN controller identifier.
 * @param  [in]  ref_id        Reference message ID.
 * @param  [in]  xtd           Whether the reference message ID is in extended format.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  TTCAN 参考消息配置
 * @param  [in]  controller_id CAN控制器标识符。
 * @param  [in]  ref_id 参考消息ID。
 * @param  [in]  xtd 参考消息ID是否为扩展格式。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_tt_ref_message_config(uint8_t controller_id, uint32_t ref_id, bool xtd);

/**
 * @if Eng
 * @brief  Configure the TTCAN basic settings.
 * @param  [in]  controller_id CAN controller identifier.
 * @param  [in]  tt_config TTCAN basic configuration.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  TTCAN 时间触发器配置
 * @param  [in]  controller_id CAN控制器标识符。
 * @param  [in]  time_trigger TTCAN时间触发器
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_can_set_time_trigger(uint8_t controller_id, can_time_trigger_t *time_trigger);
#endif

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
#endif