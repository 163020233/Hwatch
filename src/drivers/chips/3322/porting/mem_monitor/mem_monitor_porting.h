/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides MEM_Monitor port \n
 *
 * History: \n
 * 2023-03-15, Create file. \n
 */
#ifndef MEM_MONITOR_PORTING_H
#define MEM_MONITOR_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "platform_core.h"
#include "chip_io.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_mem_monitor MEM_Monitor
 * @ingroup  drivers_port
 * @{
 */

#define MEM_MONITOR_SECTION_MAX                 (0x4)
#define MEM_MONITOR_ADDREN_REG_OFFSET           (0x20)
typedef void (*uapi_tcm_monitor_callback_t)(uint32_t addr, uint32_t id);
/**
 * @if Eng
 * @brief  MEM_Monitor v150 register num.
 * @else
 * @brief  MEM_Monitor v150寄存器编号。
 * @endif
 */
typedef enum mem_monitor {
    MEM_MONITOR_NUM_0,
    MEM_MONITOR_NUM_1,
#if CORE == APPS
    MEM_MONITOR_NUM_2,
    MEM_MONITOR_NUM_3,
    MEM_MONITOR_NUM_4,
#endif
#if CORE == BT
    MEM_MONITOR_NUM_5,
    MEM_MONITOR_NUM_6,
#endif
    MEM_MONITOR_NUM_MAX
} mem_monitor_t;

/**
 * @if Eng
 * @brief  MEM_Monitor v150 section ID.
 * @else
 * @brief  MEM_Monitor v150段ID。
 * @endif
 */
typedef enum mem_monitor_section {
    MEM_MONITOR_SECTION_0,
    MEM_MONITOR_SECTION_1,
    MEM_MONITOR_SECTION_2,
    MEM_MONITOR_SECTION_3
} mem_monitor_section_t;

/**
 * @if Eng
 * @brief  Definition of the TCM_Monitor work mode.
 * @else
 * @brief  MEM_Monitor监控模式定义。
 * @endif
 */
typedef enum tcm_monitor_mode {
    TCM_MONITOR_MODE_READ = 1,              /*!< @if Eng TCM_Monitor mode: read mode.
                                                 @else   TCM_Monitor  @endif */
    TCM_MONITOR_MODE_WRITE                  /*!< @if Eng TCM_Monitor mode: write mode.
                                                 @else   TCM_Monitor  @endif */
} tcm_monitor_mode_t;

/**
 * @if Eng
 * @brief  Definition of MEM_Monitor configuration.
 * @else
 * @brief  TCM_Monitor配置类型定义。
 * @endif
 */
typedef struct tcm_monitor_config {
    mem_monitor_section_t section;          /*!< @if Eng Address section for memory monitoring.
                                                 @else   内存监控地址段。  @endif */
    uint32_t addr_start;                    /*!< @if Eng Monitoring start address.
                                                 @else   监控起始地址。  @endif */
    uint32_t addr_end;                      /*!< @if Eng Monitoring end address.
                                                 @else   监控结束地址。  @endif */
    tcm_monitor_mode_t mode;                /*!< @if Eng Memory monitoring mode.
                                                 @else   内存监控模式。  @endif */
} tcm_monitor_config_t;

typedef struct mem_monitor_region {
    uint32_t start_addr;
    uint32_t end_addr;
} mem_monitor_region_t;

typedef enum mem_monitor_addrnum {
    MEM_MONITOR_ADDRNUM_1 = 1,
    MEM_MONITOR_ADDRNUM_2,
    MEM_MONITOR_ADDRNUM_3,
    MEM_MONITOR_ADDRNUM_4
} mem_monitor_addrnum_t;

/**
 * @if Eng
 * @brief  Get the base address of a specified MEM_Monitor.
 * @return The base address of specified MEM_Monitor.
 * @else
 * @brief  获取指定MEM_Monitor的基地址。
 * @return 指定MEM_Monitor的基地址。
 * @endif
 */
uintptr_t *mem_monitor_port_base_addr_get(void);

/**
 * @if Eng
 * @brief  Register hal funcs objects into hal_mem_monitor module.
 * @else
 * @brief  将hal funcs对象注册到hal_mem_monitor模块中。
 * @endif
 */
void mem_monitor_port_register_hal_funcs(void);

/**
 * @if Eng
 * @brief  Unregister hal funcs objects from hal_mem_monitor module.
 * @else
 * @brief  从hal_mem_monitor模块注销hal funcs对象。
 * @endif
 */
void mem_monitor_port_unregister_hal_funcs(void);

/**
 * @if Eng
 * @brief  Register irq for MEM_Monitor.
 * @else
 * @brief  MEM_Monitor注册中断。
 * @endif
 */
void mem_monitor_port_register_irq(void);

/**
 * @if Eng
 * @brief  Unregister irq for MEM_Monitor.
 * @else
 * @brief  MEM_Monitor去注册中断。
 * @endif
 */
void mem_monitor_port_unregister_irq(void);

/**
 * @if Eng
 * @brief  Lock of MEM_Monitor interrupt.
 * @else
 * @brief  MEM_Monitor中断锁定。
 * @endif
 */
void mem_monitor_port_irq_lock(void);

/**
 * @if Eng
 * @brief  Unlock of mem_monitor interrupt.
 * @else
 * @brief  MEM_Monitor中断解锁。
 * @endif
 */
void mem_monitor_port_irq_unlock(void);

/**
 * @if Eng
 * @brief  Check whether the addr configured is valid.
 * @param  [in]  monitor Memory monitoring ID.
 * @param  [in]  addr_start Memory monitoring start address.
 * @param  [in]  addr_end Memory monitoring end address.
 * @return The value 'true' indicates that the addr is valid and the value 'false' indicates that the addr is invalid.
 * @else
 * @brief  检查配置的地址是否合法。
 * @param  [in]  monitor 内存监控ID。
 * @param  [in]  addr_start 内存监控起始地址。
 * @param  [in]  addr_end 内存监控结束地址。
 * @return 值“true”表示addr有效，值“false”表示addr无效。
 * @endif
 */
bool mem_monitor_port_check_addr(mem_monitor_t monitor, uint32_t addr_start, uint32_t addr_end);

#ifdef CONFIG_SUPPORT_MEM_MONITOR_MULTI_ADDRNUM
void mem_monitor_port_reg_addr_en_r(uint32_t monitor, uint32_t section);
void mem_monitor_port_reg_addr_en_w(uint32_t monitor, uint32_t section);
#endif

void mem_monitor_enable_rec_pc_trigger(void);
void mem_monitor_trigger_cb_proc(void);
errcode_t uapi_tcm_monitor_init(void);
void uapi_tcm_monitor_deinit(void);
void uapi_tcm_monitor_enable(uapi_tcm_monitor_callback_t callback);
void uapi_tcm_monitor_disable(void);
errcode_t uapi_tcm_monitor_set_section(tcm_monitor_config_t *config);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif