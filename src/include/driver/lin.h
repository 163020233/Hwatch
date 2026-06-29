/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides LIN driver API \n
 *
 * History: \n
 * 2025-03-27, Create file. \n
 */
#ifndef LIN_H
#define LIN_H

#include <stdint.h>
#include <stddef.h>

#include "hal_lin.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_lin LIN
 * @ingroup  drivers_driver
 * @{
 */

/* Frame identifiers 60 (0x3C) to 61 (0x3D) shall always use classic checksum. */
#define LIN_FRAME_MASTER_REQUEST    0x3C
#define LIN_FRAME_SLAVE_RESPONSE    0x3D

typedef enum {
    LIN_NOT_OK = 0x0,       /* Development or production error occurred */
    LIN_TX_OK,              /* Successful transmission */
    LIN_TX_BUSY,            /* Ongoing transmission (Header or Response) */
    LIN_TX_HEADER_ERROR,    /* Erroneous header transmission such as:
                               - Mismatch between sent and read back data
                               - Identifier parity error or Physical bus error */
    LIN_TX_ERROR,           /* Erroneous response transmission such as:
                               - Mismatch between sent and read back data
                               - Physical bus error */
    LIN_RX_OK,              /* Reception of correct response */
    LIN_RX_BUSY,            /* Ongoing reception: at least one response byte has been received,
                               but the checksum byte has not been received */
    LIN_RX_ERROR,           /* Erroneous response reception such as:
                               - Framing error
                               - Overrun error
                               - Checksum error or Short response */
    LIN_RX_NO_RESPONSE,     /* No response byte has been received so far */
    LIN_OPERATIONAL,        /* Normal operation; the related LIN channel is woken up
                               from the LIN_CH_SLEEP and no data has been sent. */
    LIN_CH_SLEEP            /* Sleep state operation; in this state wake-up detection
                               from slave nodes is enabled. */
} lin_status_t;

typedef enum {
    LIN_ERR_HEADER = 0x0,   /* Error in header */
    LIN_ERR_RESP_STOPBIT,   /* Framing error in response */
    LIN_ERR_RESP_CHKSUM,    /* Checksum error */
    LIN_ERR_RESP_DATABIT,   /* Monitoring error of transmitted data bit in response */
    LIN_ERR_NO_RESP,        /* No response */
    LIN_ERR_INC_RESP        /* Incomplete response */
} lin_slave_error_t;

typedef hal_lin_channel_config_t lin_channel_config_t;

typedef hal_lin_frame_t lin_frame_t;

typedef hal_lin_frame_cnt_info_t lin_frame_cnt_info_t;

typedef struct {
#ifndef CONFIG_LIN_PURE_MASTER
    /* Only used for LIN slave node */
    uint8_t (*header_indication)(uint8_t channel, lin_frame_t *frame);
    /* Only used for LIN slave node */
    void (*error_indication)(uint8_t channel, lin_slave_error_t error_status);
#endif
    void (*rx_indication)(uint8_t channel, lin_frame_t *frame);
    void (*tx_confirmation)(uint8_t channel);
    void (*wakeup_indication)(uint8_t channel);
    void (*sleep_indication)(uint8_t channel);
} lin_notifier_t;

/**
 * @if Eng
 * @brief  Initializes the specified LIN channel.
 * @param  [in]  channel LIN channel to be addressed. see @ref lin_channel_t
 * @param  [in]  cfg Pointer to the LIN channel configuration set.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @remark <b>Sync/Async:</b> Synchronous
 * @remark <b>Reentrancy:</b> Reentrant
 * @else
 * @brief  初始化指定的LIN通道。
 * @param  [in]  channel 指定的LIN通道。参考 @ref lin_channel_t
 * @param  [in]  cfg 指向LIN通道配置的指针。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @remark <b>同步/异步：</b> 同步
 * @remark <b>可重入性： </b> 可重入
 * @endif
 */
errcode_t uapi_lin_init(uint8_t channel, const lin_channel_config_t *cfg);

/**
 * @if Eng
 * @brief  Deinitializes the specified LIN channel.
 * @param  [in]  channel LIN channel to be addressed. see @ref lin_channel_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @remark <b>Sync/Async:</b> Synchronous
 * @remark <b>Reentrancy:</b> Reentrant
 * @else
 * @brief  去初始化指定的LIN通道。
 * @param  [in]  channel 指定的LIN通道。参考 @ref lin_channel_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @remark <b>同步/异步：</b> 同步
 * @remark <b>可重入性： </b> 可重入
 * @endif
 */
errcode_t uapi_lin_deinit(uint8_t channel);

/**
 * @if Eng
 * @brief  Registers the notifier callback functions.
 *         Can ONLY be called ONCE.
 * @param  [in]  notifier Pointer to the notifier structure, see @ref lin_notifier_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @remark <b>Sync/Async:</b> Synchronous
 * @remark <b>Reentrancy:</b> Reentrant
 * @else
 * @brief  注册LIN通知回调函数。
 * @param  [in]  notifier 指向包含通知回调函数的结构体。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @remark <b>同步/异步：</b> 同步
 * @remark <b>可重入性： </b> 可重入
 * @endif
 */
errcode_t uapi_lin_register_notifier(const lin_notifier_t *notifier);

/**
 * @if Eng
 * @brief  Checks if a wakeup has occurred on the addressed LIN channel.
 * @param  [in]  channel LIN channel to be addressed. see @ref lin_channel_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @remark <b>Sync/Async:</b> Synchronous
 * @remark <b>Reentrancy:</b> Reentrant
 * @else
 * @brief  检测LIN通道上是否有唤醒发生。
 * @param  [in]  channel 指定的LIN通道。参考 @ref lin_channel_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @remark <b>同步/异步：</b> 同步
 * @remark <b>可重入性： </b> 可重入
 * @endif
 */
errcode_t uapi_lin_check_wakeup(uint8_t channel);

#ifndef CONFIG_LIN_PURE_SLAVE
/**
 * @if Eng
 * @brief  Sends a LIN header and a LIN response, if necessary.
           The direction of the frame response (master response, slave response,
           slave-to-slave communication) is provided by the frame->drc.
 *         Notes: Only used for LIN master node.
 * @param  [in]  channel LIN channel to be addressed. see @ref lin_channel_t
 * @param  [in]  frame   Pointer to the @ref lin_frame_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @remark <b>Sync/Async:</b> Asynchronous
 * @remark <b>Reentrancy:</b> Reentrant
 * @else
 * @brief  如果需要，发送LIN帧头和LIN响应。
           帧响应的方向（主机响应、从机响应、从机到从机通信）由frame->drc提供。
           说明：仅用于LIN主节点。
 * @param  [in]  channel 指定的LIN通道。参考 @ref lin_channel_t
 * @param  [in]  frame   指向 @ref lin_frame_t的指针
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @remark <b>同步/异步：</b> 异步
 * @remark <b>可重入性： </b> 可重入
 * @endif
 */
errcode_t uapi_lin_send_frame(uint8_t channel, const lin_frame_t *frame);

/**
 * [SWS_Lin_00266]⌈ The function Lin_GoToSleep shall set the channel state to
 * LIN_CH_SLEEP_PENDING, even in case of an erroneous transmission of the
 * go-to-sleep-command.⌋
 *
 * [SWS_Lin_00255]⌈ The LIN channel shall enter the state LIN_CH_SLEEP the next
 * time Lin_GetStatus is called, independent of the success of the
 * transmission of the goto-sleep-command on the bus.⌋
 */
/**
 * @if Eng
 * @brief  The service instructs the driver to transmit a Go-to-sleep cmd on the LIN channel.
 *         Notes: Only used for LIN master node.
 * @param  [in]  channel LIN channel to be addressed. see @ref lin_channel_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @remark <b>Sync/Async:</b> Asynchronous
 * @remark <b>Reentrancy:</b> Reentrant
 * @else
 * @brief  该服务指示驱动程序在指定的LIN通道上发送进入睡眠状态的命令。
           说明：仅用于LIN主节点。
 * @param  [in]  channel 指定的LIN通道。参考 @ref lin_channel_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @remark <b>同步/异步：</b> 异步
 * @remark <b>可重入性： </b> 可重入
 * @endif
 */
errcode_t uapi_lin_go_to_sleep(uint8_t channel);
#endif

/**
 * @if Eng
 * @brief  Sets the channel state to LIN_CH_SLEEP, enables the wake-up detection and
           optionally sets the LIN hardware unit to reduced power operation mode
           (if supported by HW)
 * @param  [in]  channel LIN channel to be addressed. see @ref lin_channel_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @remark <b>Sync/Async:</b> Synchronous
 * @remark <b>Reentrancy:</b> Reentrant
 * @else
 * @brief  将通道状态设置为LIN_CH_SLEEP，启用唤醒检测，并选择性地将LIN硬件单元设置为
           低功耗运行模式（如果硬件支持）。
 * @param  [in]  channel 指定的LIN通道。参考 @ref lin_channel_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @remark <b>同步/异步：</b> 同步
 * @remark <b>可重入性： </b> 可重入
 * @endif
 */
errcode_t uapi_lin_go_to_sleep_internal(uint8_t channel);

/**
 * @if Eng
 * @brief  Generates a wake-up pulse and sets the channel state to LIN_CH_OPERATIONAL.
 * @param  [in]  channel LIN channel to be addressed. see @ref lin_channel_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @remark <b>Sync/Async:</b> Asynchronous
 * @remark <b>Reentrancy:</b> Reentrant
 * @else
 * @brief  生成一个唤醒信号，并将通道状态设置为LIN_CH_OPERATIONAL。
 * @param  [in]  channel 指定的LIN通道。参考 @ref lin_channel_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @remark <b>同步/异步：</b> 异步
 * @remark <b>可重入性： </b> 可重入
 * @endif
 */
errcode_t uapi_lin_wakeup(uint8_t channel);

/**
 * @if Eng
 * @brief  Sets the channel state to LIN_CH_OPERATIONAL without generating a wake-up pulse.
 * @param  [in]  channel LIN channel to be addressed. see @ref lin_channel_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @remark <b>Sync/Async:</b> Asynchronous
 * @remark <b>Reentrancy:</b> Reentrant
 * @else
 * @brief  将通道状态设置为LIN_CH_OPERATIONAL，但不生成唤醒信号。
 * @param  [in]  channel 指定的LIN通道。参考 @ref lin_channel_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @remark <b>同步/异步：</b> 异步
 * @remark <b>可重入性： </b> 可重入
 * @endif
 */
errcode_t uapi_lin_wakeup_internal(uint8_t channel);

#ifndef CONFIG_LIN_PURE_SLAVE
/**
 * [SWS_Lin_00238] ⌈ The function Lin_GetStatus shall return LIN_TX_OK, when
 * - A Master Response Type frame is send and LIN header as well as LIN
 * response of the frame are transmitted successfully or
 * - A Slave to Slave Response Type frame is send and the LIN header of the
 * frame is transmitted successfully.⌋
 *
 * [SWS_Lin_00092] ⌈ If a SDU has been successfully received, the function
 * Lin_GetStatus shall store the SDU in a shadow buffer or memory mapped LIN
 * Hardware receive buffer referenced by Lin_SduPtr. The buffer will only be
 * valid and must be read until the next Lin_SendFrame function call.⌋
 */
/**
 * @if Eng
 * @brief  Get the status of the LIN driver.
 *         Notes: Only used for LIN master node.
 * @param  [in]  channel LIN channel to be addressed. see @ref lin_channel_t
 * @param  [out] status  Pointer to pointer to a @lin_status_t.
 * @param  [out] sdu_ptr Pointer to pointer to a shadow buffer or memory mapped LIN
                         Hardware receive buffer where the current SDU is stored.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @remark <b>Sync/Async:</b> Synchronous
 * @remark <b>Reentrancy:</b> Reentrant
 * @else
 * @brief  获取LIN驱动的状态。
           说明：仅用于LIN主节点。
 * @param  [in]  channel 指定的LIN通道。参考 @ref lin_channel_t
 * @param  [out] status  指向lin_status_t的指针。
 * @param  [out] sdu_ptr 指向影子缓冲区或内存映射LIN的指针，存储当前SDU的硬件接收缓冲区。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @remark <b>同步/异步：</b> 同步
 * @remark <b>可重入性： </b> 可重入
 * @endif
 */
errcode_t uapi_lin_get_status(uint8_t channel, lin_status_t *status, uint8_t **sdu_ptr);
#endif

#ifdef CONFIG_LIN_SUPPORT_LOOPBACK
 /**
 * @if Eng
 * @brief  Configures the LIN channel loopback feature.
 * @param  [in]  ch0 LIN loopback channel 0. see @ref lin_channel_t
 * @param  [in]  ch1 LIN loopback channel 1. see @ref lin_channel_t
 * @param  [in]  on  Enable or disable the loopback feature.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  配置LIN通道的环回功能。
 * @param  [in]  ch0 环回通道0。参考 @ref lin_channel_t
 * @param  [in]  ch1 环回通道1。参考 @ref lin_channel_t
 * @param  [in]  on  启用或禁用环回功能。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_lin_loopback_config(uint8_t ch0, uint8_t ch1, bool on);
#endif

#ifdef CONFIG_LIN_SUPPORT_FRAME_COUNT
/**
 * @if Eng
 * @brief  Configures the frame count feature for the specified LIN channel.
 * @param  [in]  channel LIN channel to be addressed. see @ref lin_channel_t
 * @param  [in]  on      Enable or disable the frame count feature.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @remark <b>Sync/Async:</b> Synchronous
 * @remark <b>Reentrancy:</b> Reentrant
 * @else
 * @brief  配置指定LIN通道的帧计数功能。
 * @param  [in]  channel 指定的LIN通道。参考 @ref lin_channel_t
 * @param  [in]  on      启用或禁用帧计数功能。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @remark <b>同步/异步：</b> 同步
 * @remark <b>可重入性： </b> 可重入
 * @endif
 */
errcode_t uapi_lin_frame_cnt_config(uint8_t channel, bool on);

/**
 * @if Eng
 * @brief  Retrieves frame count information for the specified LIN channel.
 * @param  [in]  channel LIN channel to be addressed. see @ref lin_channel_t
 * @param  [out] info    Pointer to a structure where frame count information is stored.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @remark <b>Sync/Async:</b> Synchronous
 * @remark <b>Reentrancy:</b> Reentrant
 * @else
 * @brief  获取指定LIN通道的帧计数信息。
 * @param  [in]  channel 指定的LIN通道。参考 @ref lin_channel_t
 * @param  [out] info    指向存储帧计数信息的结构体。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @remark <b>同步/异步：</b> 同步
 * @remark <b>可重入性： </b> 可重入
 * @endif
 */
errcode_t uapi_lin_get_frame_cnt_info(uint8_t channel, lin_frame_cnt_info_t *info);
#endif

#ifdef CONFIG_LIN_SUPPORT_LPM
void uapi_lin_suspend(void);
void uapi_lin_resume(void);
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
