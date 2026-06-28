/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides mipi driver api
 *
 * History:
 * Create: 2022-04-14
 */

#ifndef SOC_MIPI_TX_H
#define SOC_MIPI_TX_H

#include <stdint.h>
#include "td_base.h"
#include "platform_core.h"
#include "platform_section.h"
#include "soc_lcd_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MIPI_TX_DISABLE_LANE_ID (-1)
typedef struct {
    mipi_tx_work_mode_t work_mode;  /*!< @if Eng work mode: low power mode, high speed mode.
                                         @else   工作模式：低功耗模式、高速模式。@endif */
    uint8_t             devno;      /*!< @if Eng device number.
                                         @else   设备编号。@endif */
    uint16_t            data_type;  /*!< @if Eng data type.
                                         @else   数据类型。@endif */
    uint16_t            cmd_size;
    uint8_t             *cmd;
} cmd_info_t;

typedef struct {
    mipi_tx_work_mode_t work_mode;      /*!< @if Eng work mode: low power mode, high speed mode.
                                             @else   工作模式：低功耗模式、高速模式。@endif */
    uint8_t             devno;          /*!< @if Eng device number.
                                             @else   设备编号。@endif */
    uint16_t            data_type;      /*!< @if Eng data type.
                                             @else   数据类型。@endif */
    uint16_t            data_param;     /*!< @if Eng ata param,low 8 bit:first param.
                                                     high 8 bit:second param, set 0 if not use
                                             @else   数据参数，低8位：第一个参数。
                                                     高8位：第二个参数，如果不使用，则设置0。@endif */
    uint16_t            get_data_size;  /*!< @if Eng data type.
                                             @else   读取数据大小。@endif */
    uint8_t             *get_data;      /*!< @if Eng read data memery address, should malloc by user.
                                             @else   读取数据内存地址，由使用者分配。@endif */
} get_cmd_info_t;

typedef enum {
    MIPI_TX_EXIT_ULPS_CNT_ADD = 0,
    MIPI_TX_EXIT_ULPS_CNT_SUB
} mipi_tx_exit_ulps_ext_opt;

PLT_SEC_ITCM_T static inline void ext_fence(void)
{
    __asm__ __volatile__("fence":::"memory");
}

/**
 * @if Eng
 * @brief  Initialize mipi tx.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  mipi tx初始化
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
uint32_t uapi_mipi_tx_init(void);

/**
 * @if Eng
 * @brief  Deinitializes mipi tx.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  mipi tx去初始化。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
uint32_t uapi_mipi_tx_deinit(void);

/**
 * @if Eng
 * @brief  Configure mipi tx controller and phy.
 * @param  [in]  dev_cfg mipi tx params. For details, see @ref combo_dev_cfg_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  配置mipi tx的控制器和phy。
 * @param  [in]  dev_cfg mipi tx配置参数，参考 @ref combo_dev_cfg_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
uint32_t uapi_mipi_tx_set_dev_cfg(combo_dev_cfg_t *dev_cfg);

/**
 * @if Eng
 * @brief  Configure mipi tx peripheral via DSI write command.
 * @param  [in]  cmd_info DSI write command params. For details, see @ref cmd_info_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  通过DSI写命令配置mipi tx从设备。
 * @param  [in]  cmd_info mipi DSI写命令参数，参考 @ref cmd_info_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
uint32_t uapi_mipi_tx_set_cmd(cmd_info_t *cmd_info);

/**
 * @if Eng
 * @brief  Read mipi tx peripheral imformation via DSI read command.
 * @param  [in]  cmd_info DSI read command params. For details, see @ref cmd_info_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  通过DSI读命令获取mipi tx从设备此信息。
 * @param  [in]  cmd_info mipi DSI读命令参数，参考 @ref cmd_info_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
uint32_t uapi_mipi_tx_get_cmd(get_cmd_info_t *cmd_info);

/**
 * @if Eng
 * @brief  Enable mipi tx pixel data transmission.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  使能mipi tx像素传输。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
uint32_t uapi_mipi_tx_enable(void);

/**
 * @if Eng
 * @brief  Disable mipi tx pixel data transmission.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  关闭mipi tx像素传输。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
uint32_t uapi_mipi_tx_disable(void);

/**
 * @if Eng
 * @brief  Set mipi_tx self define low power mode.
 * @par Description: In this mode, the DPU controls whether mipi_tx enters low power mode.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置mipi_tx自定义低功耗模式。
 * @par 说明: 该模式下，由DPU来控制mipi_tx是否进入低功耗。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
uint32_t uapi_mipi_tx_auto_enter_sdlp(void);

/**
 * @if Eng
 * @brief  Software controls mipi_tx to exit self define low power mode.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  软件控制mipi_tx退出自定义低功耗模式。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
uint32_t uapi_mipi_tx_exit_sdlp(void);

/**
 * @if Eng
 * @brief  Software controls mipi_tx to exit ultra low power state.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  软件控制mipi_tx退出ulps。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
uint32_t uapi_mipi_tx_exit_ulps(void);

/**
 * @if Eng
 * @brief  Software controls mipi_tx to enter ultra low power state by clearing exit signal.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  软件通过清除ulps退出信号使mipi_tx重新进入ulps。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
uint32_t uapi_mipi_tx_clear_exit_ulps(void);

/**
 * @if Eng
 * @brief  Software controls enabling/disabling auto exit ULPS.
 * @param  [in]  en Whether to enable (true) or disable (false) auto exit ULPS.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  软件控制开关自动退ulps功能
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
uint32_t uapi_mipi_tx_set_auto_exit_ulps_en(td_bool en);

/**
 * @if Eng
 * @brief  Software force controls mipi_tx to exit ultra low power state.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  软件控制mipi_tx强制立即退出ulps。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
uint32_t uapi_mipi_tx_force_exit_ulps(void);

/**
 * @if Eng
 * @brief  Software controls the interval between the ulpsflg signal and TEsignal.
 * @param  [in]  ulpsflg2te_time the interval between the ulpsflg signal and TEsignal. The unit is microsecond.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  软件控制ulps_flg到te信号之间的间隔(单位：us)。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
uint32_t uapi_mipi_tx_hal_set_ulpsflg2te_time(uint32_t ulpsflg2te_time_us);

/**
 * @if Eng
 * @brief  DFX setting for external ulps count (increase or decrease by 1).
 * @param  [in]  opt ulps count increase or decrease by 1.
 * @else
 * @brief  DFX设置外部退ulps计数(增加或减少1)。
 * @endif
 */
td_void uapi_mipi_tx_dfx_set_exit_ulps_cnt_ext(mipi_tx_exit_ulps_ext_opt opt);

/**
 * @if Eng
 * @brief  ULPS status anomaly monitoring during suspension.
 * @else
 * @brief  suspend状态时ulps状态异常监测。
 * @endif
 */
td_void uapi_mipi_tx_suspend_status_err_det(void);

/**
 * @if Eng
 * @brief  mipi ulps proc.
 * @else
 * @brief  mipi ulps相关proc打印
 * @endif
 */
td_void uapi_mipi_tx_ulps_proc(void);

/**
 * @if Eng
 * @brief  mipi payload error detect.
 * @else
 * @brief  mipi payload异常检测
 * @endif
 */
uint32_t uapi_mipi_tx_pld_err_det(td_bool *is_err);

td_void uapi_mipi_tx_set_bypass(td_bool bypass_en);

/**
 * @if Eng
 * @brief  mipi ulps entry suspend.
 * @else
 * @brief  mipi ulps 进入暂停
 * @endif
 */
uint32_t uapi_mipi_tx_ulps_entry_suspend(td_bool en);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* SOC_MIPI_TX_H */
