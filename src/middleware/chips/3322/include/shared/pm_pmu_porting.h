/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: pm pmu manage header file
 */

#ifndef PM_PMU_PORTING_H
#define PM_PMU_PORTING_H

#include <stdbool.h>
#include <stdint.h>
#include "pmu_buck.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    // !< Buck ldo
    PMU_BUCK_LDO_BASE,
    PMU_BUCK_LDO_BUCK_1P9 = PMU_BUCK_LDO_BASE, // buck 0
    PMU_BUCK_LDO_BUCK_0P8,                     // buck 1
    PMU_BUCK_LDO_BUCK_1P0,                     // buck 2
    PMU_BUCK_LDO_END = PMU_BUCK_LDO_BUCK_1P0,

    // !< Soc ldo
    PMU_LDO_BASE,
    PMU_LDO_MIPILDO_0P8 = PMU_LDO_BASE,
    PMU_LDO_MIPILDO_1P8,
    PMU_LDO_IOLDO0,         // ioldo0 sysldo > ulpaon
    PMU_LDO_IOLDO1,
    PMU_LDO_IOLDO2,         // auxadc
    PMU_LDO_IOLDO3,
    PMU_LDO_IOLDO4,         // ulpldo
    PMU_LDO_IOLDO6,         // ulpldo
    PMU_LDO_CLKLDO0,        // analdo0
    PMU_LDO_PLL_HVLDO,      // paldo0
    PMU_LDO_XLDO,
    PMU_LDO_INTLDO2,
    PMU_LDO_CLDO1,
    PMU_LDO_END = PMU_LDO_CLDO1,

    // !< Power Control PMU SW
    PMU_LOAD_SWITCH_BASE,
    PMU_LOAD_SWITCH_SW1P8_0 = PMU_LOAD_SWITCH_BASE,     // IOLDO1 SW0 nandflash/psram
    PMU_LOAD_SWITCH_SW1P8_1,                            // IOLDO1 SW1 tsensor
    PMU_LOAD_SWITCH_SW1P8_2,                            // IOLDO1 SW2 efuse
    PMU_LOAD_SWITCH_SW1P8_3,                            // IOLDO1 SW3 usb
    PMU_LOAD_SWITCH_SW3P3_0,                            // IOLDO3 SW3
    PMU_LOAD_SWITCH_SW0P9_1,                            // VDD_DBB -> SW0P9_1 -> ULP_AON
    PMU_LOAD_SWITCH_END = PMU_LOAD_SWITCH_SW0P9_1,

    PMU_LDO_MAX = PMU_LDO_END,
} pmu_ldo_id_t;

typedef enum {
    PMU_BUCK_LDO_STATE_OFF = 0x00,
    PMU_BUCK_LDO_STATE_ON  = 0x01,
    PMU_BUCK_LDO_STATE_ECO = 0x11,

    PMU_TURN_OFF           = 0,
    PMU_TURN_ON            = 1,

    PMU_MTCMOS_FRC_OFF     = 0x0,
    PMU_MTCMOS_FRC_ON      = 0x1,
    PMU_MTCMOS_SW_VETO_OFF = 0x2,
    PMU_MTCMOS_SW_VETO_ON  = 0x3,
    PMU_MTCMOS_HW_CONTROL  = 0x4,
} pmu_ldo_state_t;

typedef enum {
    // !< Power control PMU Sub
    PMU_BT_SUB                           = 0x0,
    PMU_AUDIO_SUB                        = 0x1,
    PMU_DISPLAY_SUB                      = 0x2,
    PMU_VAU_SUB                          = 0x3,
    PMU_H264_SUB                         = 0x4,
    PMU_MCU_HS_PERP_SUB                  = 0x5,

    // !< Power control MCU Device
    PMU_DEV_M_BASE,
    PMU_DEV_M_TIMER = PMU_DEV_M_BASE,
    PMU_DEV_M_MDMA,
    PMU_DEV_M_SMDMA,
    PMU_DEV_M_I2C0,
    PMU_DEV_M_I2C1,
    PMU_DEV_M_I2C2,
    PMU_DEV_M_I2C3,
    PMU_DEV_RSV,
    PMU_DEV_M_GPIO,
    PMU_DEV_RSV1,
    PMU_DEV_M_UART_H1,
    PMU_DEV_M_SPI1,
    PMU_DEV_M_SPI0,
    PMU_DEV_M_UART_H0,
    PMU_DEV_M_UART_L0,
    PMU_DEV_M_PWM,

    // !< Power control AON Device
    PMU_DEV_AON_BASE,
    PMU_DEV_AON_M_GPIO0 = PMU_DEV_AON_BASE,
    PMU_DEV_AON_M_GPIO1,
    PMU_DEV_AON_B_GPIO0,
    PMU_DEV_AON_DSP_GPIO0,

    PMU_CONTROL_MAX,
} pmu_control_type_t;

typedef enum {
    PMU_MEM_ITCM_1_RAM,
    PMU_MEM_ITCM_2_RAM,
    PMU_MEM_DTCM_1_RAM,
    PMU_MEM_DTCM_2_RAM,
    PMU_MEM_DISPLAY_L2M_RAM,
    PMU_MEM_NPU_L2M_0_RAM,
    PMU_MEM_NPU_L2M_1_RAM,
    PMU_MEM_NPU_L2M_2_RAM,
    PMU_MEM_GMMU_LUT_L2M_RAM,
    PMU_MEM_COM_RAM,
    PMU_MEM_BT_1_RAM,
    PMU_MEM_BT_2_RAM,
    PMU_MEM_BT_3_RAM,
    PMU_MEM_BT_4_RAM,
    PMU_MEM_BT_1_EM,
    PMU_MEM_BT_2_EM,
} pmu_mem_type_t;

typedef enum {
    PMU_POWER_OFF           = 0,
    PMU_POWER_ON_AND_RUN    = 1,
    PMU_POWER_SLEEP         = 2,
    PMU_POWER_RESUME        = 3,
    PMU_POWER_MAX           = 4,

    PMU_DEV_RESET           = 0,
    PMU_DEV_DERESET         = 1,
    PMU_DEV_ENTER_LOW_POWER = 2,
    PMU_DEV_EXIT_LOW_POWER  = 3,

    PMU_MEM_TURN_ON         = 0,
    PMU_MEM_RETENTION       = 1,
    PMU_MEM_SHUT_DOWN       = 2,
    PMU_MEM_SLEEP_SHUT_DOWN = 3,
    PMU_MEM_SLEEP_RETENTION = 4,
} pmu_control_param_t;

/**
 * @if Eng
 * @brief  Initializing the PMU Driver.
 * @else
 * @brief  初始化 PMU 驱动
 * @endif
 */
void uapi_pmu_init(void);

/**
 * @if Eng
 * @brief  Set the LDO voltage.
 * @param  [in]  id The LDO to set sleep voltage.
 * @param  [in]  vset The voltage value to be configured.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置LDO的睡眠电压。
 * @param  [in]  id 用来设置电压的LDO。
 * @param  [in]  vset 要配置的电压值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pmu_ldo_set_sleep_voltage(pmu_ldo_id_t id, uint8_t vset);

/**
 * @if Eng
 * @brief  Get the LDO sleep voltage.
 * @param  [in]  id The LDO to get the voltage.
 * @return The LDO current voltage.
 * @else
 * @brief  获取LDO睡眠电压。
 * @param  [in]  id 用来获取电压的LDO。
 * @return LDO当前睡眠电压值。
 * @endif
 */
uint8_t uapi_pmu_ldo_get_sleep_voltage(pmu_ldo_id_t id);

/**
 * @if Eng
 * @brief  Set the LDO state.
 * @param  [in]  id The LDO to set the state.
 * @param  [in]  state the state value to be configured.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  配置LDO电压。
 * @param  [in]  id 用来配置状态的LDO。
 * @param  [in]  state 用于配置的状态值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pmu_ldo_set_state(pmu_ldo_id_t id, uint8_t state);

/**
 * @if Eng
 * @brief  open or close the Modules.
 * @param  [in]  type the modules of pmu_control_type_t.
 * @param  [in]  param Enabled or disabled.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置模块上下电状态。
 * @param  [in]  type 设置的模块。
 * @param  [in]  param 配置模块的上下电状态。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pmu_control(pmu_control_type_t type, uint8_t param);

/**
 * @if Eng
 * @brief The system enters the shipmode mode.
 * @param arg Reserved parameter.
 * @else
 * @brief 系统进shipmode
 * @param arg 保留参数.
 * @endif
 */
void uapi_sys_shipmode(uint32_t arg);

/**
 * @if Eng
 * @brief  Set the usb power state.
 * @param  [in]  state the state value to be configured.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置USB 上电装填。
 * @param  [in]  state 用于配置的状态值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pmu_usb_power_control(uint8_t state);

/**
 * @}
 */
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif