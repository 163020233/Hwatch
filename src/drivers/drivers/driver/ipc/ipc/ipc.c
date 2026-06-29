/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides hal ipc \n
 *
 * History: \n
 * 2022-11-14, Create file. \n
 */

#include <stdbool.h>
#include "common_def.h"
#ifdef SUPPORT_CPU_TRACE
#include "platform_types.h"
#include "print_config.h"
#ifdef SUPPORT_CPU_TRACE_V2
#include "cpu_trace_unified.h"
#else
#include "cpu_trace.h"
#endif
#endif
#ifdef CONFIG_IPC_SUPPORT_SLAVE_POWER_STATUS
#include "connectivity_log.h"
#endif
#include "ipc_porting.h"
#if defined(CONFIG_IPC_USING_SW_MAILBOX)
#include "ipc_sw_mail.h"
#elif defined(CONFIG_IPC_USING_RING_BUFFER_MAILBOX)
#include "ipc_ring.h"
#endif
#include "ipc.h"

static bool g_ipc_inited = false;
STATIC bool ipc_is_slave_power_on(uint8_t dst)
{
#ifdef CONFIG_IPC_SUPPORT_SLAVE_POWER_STATUS
    slave_cpu_t cpu = SLAVE_CPU_MAX_NUM;
    if (dst == CORES_BT_CORE) {
        cpu = SLAVE_CPU_BT;
#if DSP_EXIST == YES
    } else if (dst == CORES_PROTOCOL_CORE) {
        cpu = SLAVE_CPU_DSP0;
#if DUAL_DSP_EXIST == YES
    } else if (dst == CORES_HIFI1_CORE) {
        cpu = SLAVE_CPU_DSP1;
#endif
#endif
#if defined(GNSS_EXIST) &&  (GNSS_EXIST == YES)
    } else if (dst == CORES_GNSS_CORE) {
        cpu = SLAVE_CPU_GNSS;
#endif
#if defined(WIFI_EXIST) &&  (WIFI_EXIST == YES)
    } else if (dst == CORES_WIFI_CORE) {
        cpu = SLAVE_CPU_WIFI;
#endif
#if (defined(SENSOR_EXIST) && SENSOR_EXIST == YES)
    } else if (dst == CORES_SEN_CORE) {
        cpu = SLAVE_CPU_SENSOR;
#endif
#if (defined(CCORE_EXIST) && CCORE_EXIST == YES)
    } else if (dst == CORES_CO_CORE) {
        cpu = SLAVE_CPU_CCORE;
#endif
    } else { // 主核默认一直开启
        return true;
    }
    return get_slave_cpu_power_mode(cpu) == SLAVE_CPU_POWER_ON;
#else
    unused(dst);
    return true;
#endif
}

#ifdef SUPPORT_CPU_TRACE_V2
#define TRACED_BCPU_INDEX   0
#define TRACED_MCPU_INDEX   1
#define TRACED_SCPU_INDEX   2
#define TRACED_WCPU_INDEX   3
#define TRACED_CCPU_INDEX   4
#define TRACED_DSP0_INDEX   5
STATIC void ipc_error_print_trace_v2(const ipc_msg_info_t *msg_info)
{
    uint32_t traced_cpu = CORES_BT_CORE;
    if  (msg_info->dst_core == CORES_BT_CORE) {
        traced_cpu = TRACED_BCPU_INDEX;
    } else if (msg_info->dst_core == CORES_APPS_CORE) {
        traced_cpu = TRACED_MCPU_INDEX;
#if (defined(SENSOR_EXIST) && SENSOR_EXIST == YES)
    } else if (msg_info->dst_core == CORES_SEN_CORE) {
        traced_cpu = TRACED_SCPU_INDEX;
#endif
#if (defined(WIFI_EXIST) && WIFI_EXIST == YES)
    } else if (msg_info->dst_core == CORES_WIFI_CORE) {
        traced_cpu = TRACED_WCPU_INDEX;
#endif
#if (defined(CCORE_EXIST) && CCORE_EXIST == YES)
    } else if (msg_info->dst_core == CORES_CO_CORE) {
        traced_cpu = TRACED_CCPU_INDEX;
#endif
#if (defined(DSP_EXIST) && DSP_EXIST == YES)
    } else if (msg_info->dst_core == CORES_PROTOCOL_CORE) {
        traced_cpu = TRACED_DSP0_INDEX;
#endif
    } else {
        return;
    }
    uapi_cpu_trace_lock_pclr(traced_cpu);
    uint32_t pc = uapi_cpu_trace_get_locked_pc(traced_cpu);
    uint32_t lr = uapi_cpu_trace_get_locked_lr(traced_cpu);
    uint32_t sp = uapi_cpu_trace_get_locked_sp(traced_cpu);
    oam_err_log("traced_cpu %d current pc=0x%x, lr=0x%x, sp=0x%x", traced_cpu, pc, lr, sp);
}
#endif

STATIC void ipc_send_failed(errcode_t failed_reason, const ipc_msg_info_t *msg_info)
{
#if (defined(GNSS_EXIST) && GNSS_EXIST == YES)
    // hso log causes GNSS services to fail
    if (CORE == CORES_GNSS_CORE) {
        return;
    }
#endif
    if (msg_info->msg_id == IPC_MSG_LOG_INFO) {
        return;
    }
#ifdef SUPPORT_CPU_TRACE
    oam_err_log("ipc error 0x%x: core %d => %d, id=%d ", failed_reason, CORE, msg_info->dst_core, msg_info->msg_id);
#ifdef SUPPORT_CPU_TRACE_V2
    ipc_error_print_trace_v2(msg_info);
#else
    hal_cpu_trace_traced_cpu_t lock_cpu = HAL_CPU_TRACE_TRACED_MCPU;
    if  (msg_info->dst_core == CORES_BT_CORE) {
        lock_cpu = HAL_CPU_TRACE_TRACED_BCPU;
    } else if (msg_info->dst_core == CORES_APPS_CORE) {
        lock_cpu = HAL_CPU_TRACE_TRACED_MCPU;
#if (defined(GNSS_EXIST) && GNSS_EXIST == YES)
    } else if (msg_info->dst_core == CORES_GNSS_CORE) {
        lock_cpu = HAL_CPU_TRACE_TRACED_GCPU;
#endif
    } else {
        return;
    }
    /* lock cpu */
    uint32_t pc = 0;
    uint32_t lr = 0;
    uint32_t sp = 0;
    cpu_trace_get_locked_regs(lock_cpu, &pc, &lr, &sp);
    oam_err_log("core %d current pc=0x%x, lr=0x%x, sp=0x%x", msg_info->dst_core, pc, lr, sp);
#endif
#else
    unused(failed_reason);
#endif
}

errcode_t uapi_ipc_init(void)
{
    errcode_t ret = ERRCODE_FAIL;

    if (unlikely(g_ipc_inited)) {
        return ERRCODE_SUCC;
    }
#if defined(CONFIG_IPC_SUPPORT_BUFFER_INIT)
    ipc_port_buffer_init();
#endif

#if defined(CONFIG_IPC_USING_SW_MAILBOX)
    ret = ipc_sw_init();
#elif defined(CONFIG_IPC_USING_RING_BUFFER_MAILBOX)
    ret = ipc_ring_init();
#endif
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    g_ipc_inited = true;
    return ERRCODE_SUCC;
}

errcode_t uapi_ipc_deinit(void)
{
    errcode_t ret = ERRCODE_FAIL;

    if (unlikely(!g_ipc_inited)) {
        return ERRCODE_SUCC;
    }
#if defined(CONFIG_IPC_USING_SW_MAILBOX)
    ret = ipc_sw_deinit();
#elif defined(CONFIG_IPC_USING_RING_BUFFER_MAILBOX)
    ret = ipc_ring_deinit();
#endif
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    g_ipc_inited = false;
    return ERRCODE_SUCC;
}

errcode_t uapi_ipc_send_msg_sync(ipc_msg_info_t *msg_info)
{
    errcode_t ret = ERRCODE_FAIL;

    if (unlikely(!g_ipc_inited)) {
        return ERRCODE_IPC_NOT_INIT;
    }
    if (msg_info == NULL) {
        return ERRCODE_IPC_INVALID_PARAMETER;
    }
    if (ipc_is_slave_power_on(msg_info->dst_core) == false) {
        return ERRCODE_IPC_SLAVE_POWER_OFF;
    }
#if defined(CONFIG_IPC_USING_SW_MAILBOX)
    ret = ipc_sw_send_msg_sync(msg_info);
#elif defined(CONFIG_IPC_USING_RING_BUFFER_MAILBOX)
    ret = ipc_ring_send_msg_sync(msg_info);
#endif
    if (ret != ERRCODE_SUCC) {
        ipc_send_failed(ret, msg_info);
    }
    return ret;
}

errcode_t uapi_ipc_send_msg_async(ipc_msg_info_t *msg_info)
{
    errcode_t ret = ERRCODE_FAIL;

    if (unlikely(!g_ipc_inited)) {
        return ERRCODE_IPC_NOT_INIT;
    }
    if (msg_info == NULL) {
        return ERRCODE_IPC_INVALID_PARAMETER;
    }
    if (ipc_is_slave_power_on(msg_info->dst_core) == false) {
        return ERRCODE_IPC_SLAVE_POWER_OFF;
    }
#if defined(CONFIG_IPC_USING_SW_MAILBOX)
    ret = ipc_sw_send_msg_async(msg_info);
#elif defined(CONFIG_IPC_USING_RING_BUFFER_MAILBOX)
    ret = ipc_ring_send_msg_async(msg_info);
#endif
    if (ret != ERRCODE_SUCC) {
        ipc_send_failed(ret, msg_info);
    }
    return ret;
}

errcode_t uapi_ipc_register_rx_handler(ipc_rx_handler_info_t *handler_info)
{
    errcode_t ret = ERRCODE_FAIL;

    if (unlikely(!g_ipc_inited)) {
        return ERRCODE_IPC_NOT_INIT;
    }
    if (handler_info == NULL) {
        return ERRCODE_IPC_INVALID_PARAMETER;
    }
#if defined(CONFIG_IPC_USING_SW_MAILBOX)
    ret = ipc_sw_register_rx_handler(handler_info);
#elif defined(CONFIG_IPC_USING_RING_BUFFER_MAILBOX)
    ret = ipc_ring_register_rx_handler(handler_info);
#endif
    return ret;
}
