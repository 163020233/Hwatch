/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description:   CPU Trace Driver Header File
 */
#ifndef CPU_TRACE_H
#define CPU_TRACE_H

#include "hal_cpu_trace.h"
#include "errcode.h"
#include "core.h"

/**
 * @defgroup connectivity_drivers_non_os_cpu_trace CPU Trace
 * @ingroup  connectivity_drivers_non_os
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of the structure of CPU trace records
 * @else
 * @brief  记录CPU trace的结构定义。
 * @endif
 */
typedef struct cpu_trace_item {
    uint32_t time;                  /*!< @if Eng timestamp.
                                         @else   时间戳。  @endif */
    uint32_t lr;                    /*!< @if Eng LR/RA register of the next instruction to be executed by the CPU.
                                         @else   CPU下一条执行指令寄存器LR/RA。  @endif */
    uint32_t pc;                    /*!< @if Eng CPU currently executed instruction register PC.
                                         @else   CPU当前执行指令寄存器PC。  @endif */
} cpu_trace_item_t;

/**
 * @if Eng
 * @brief  Enable CPU Trace.
 * @param  [in]  cpu core ID set by the CPU.
 * @else
 * @brief  使能CPU Trace。
 * @param  [in]  cpu 设置的CPU核心编号。
 * @endif
 */
void uapi_cpu_trace_enable(uint32_t cpu);

/**
 * @if Eng
 * @brief  Disable CPU Trace.
 * @param  [in]  cpu core ID set by the CPU.
 * @else
 * @brief  关闭CPU Trace。
 * @param  [in]  cpu 设置的CPU核心编号。
 * @endif
 */
void uapi_cpu_trace_disable(uint32_t cpu);

/**
 * @if Eng
 * @brief  Enable cpu Trace again.
 * @param  [in]  CPU core ID set by the CPU.
 * @else
 * @brief  重新使能CPU Trace。
 * @param  [in]  cpu 设置的CPU核心编号。
 * @endif
 */
void uapi_cpu_trace_enable_repeat(uint32_t cpu);

/**
 * @if Eng
 * @brief  Lock&store the current PC and LR.
 * @param  [in]  cpu core ID set by the CPU.
 * @else
 * @brief  锁存当前的PC和LR。
 * @param  [in]  cpu 设置的CPU核心编号。
 * @endif
 */
void uapi_cpu_trace_lock_pclr(uint32_t cpu);

/**
 * @if Eng
 * @brief  Get stored PC.
 * @param  [in]  cpu core ID set by the CPU.
 * @retval the value of PC.
 * @else
 * @brief  获取存储的PC。
 * @param  [in]  cpu 设置的CPU核心编号。
 * @retval PC值。
 * @endif
 */
uint32_t uapi_cpu_trace_get_locked_pc(uint32_t cpu);

/**
 * @if Eng
 * @brief  Get stored LR.
 * @param  [in]  cpu core ID set by the CPU.
 * @retval the value of LR.
 * @else
 * @brief  获取存储的LR。
 * @param  [in]  cpu 设置的CPU核心编号。
 * @retval LR值。
 * @endif
 */
uint32_t uapi_cpu_trace_get_locked_lr(uint32_t cpu);

/**
 * @if Eng
 * @brief  Get stored SP.
 * @param  [in]  cpu core ID set by the CPU.
 * @retval the value of SP.
 * @else
 * @brief  获取存储的SP。
 * @param  [in]  cpu 设置的CPU核心编号。
 * @retval SP值。
 * @endif
 */
uint32_t uapi_cpu_trace_get_locked_sp(uint32_t cpu);

/**
 * @if Eng
 * @brief  Get stored LR.
 * @param  [in]  cpu core ID set by the CPU.
 * @param  [out]  pc the value of PC.
 * @param  [out]  lr the value of LR.
 * @param  [out]  sp the value of SP.
 * @retval true SUCCESS.
 * @retval false FAILURE.
 * @else
 * @brief  获取存储的LR。
 * @param  [in]  cpu 设置的CPU核心编号。
 * @param  [out]  PC值。
 * @param  [out]  LR值。
 * @param  [out]  SP值。
 * @retval true 成功。
 * @retval false 失败。
 * @endif
 */
bool uapi_cpu_trace_get_locked_regs(uint32_t cpu, uint32_t *pc, uint32_t *lr, uint32_t *sp);

/**
 * @if Eng
 * @brief  Sets the pass-through function of the locked pclr.
 * @param  [in]  cpu core ID set by the CPU.
 * @retval the value of SP.
 * @else
 * @brief  设置pclr锁存的直通功能。
 * @param  [in]  cpu 设置的CPU核心编号。
 * @retval SP值。
 * @endif
 */
void uapi_cpu_trace_pclr_set_passthrough(uint32_t cpu);

#if defined(CONFIG_CPU_TRACE_SUPPORT_LPM)
/**
 * @if Eng
 * @brief  Suspend all of the MCPU trace channels.
 * @param  [in]  arg Argument for suspend.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  停止cpu trace功能。.
 * @param  [in]  arg 停止的参数.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_cpu_trace_suspend(uintptr_t arg);

/**
 * @brief  Resume all of the MCPU trace channels.
 * @param  [in]  arg Argument for resume, Which core need to resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 */
/**
 * @if Eng
 * @brief  Resume all of the MCPU trace channels.
 * @param  [in]  arg Argument for resume, Which core need to resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复cpu trace功能。.
 * @param  [in]  arg 恢复的参数.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_cpu_trace_resume(uintptr_t arg);
#endif

/**
 * @}
 */
#endif
