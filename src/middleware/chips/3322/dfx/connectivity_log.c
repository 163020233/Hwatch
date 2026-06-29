/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: logging interface
 * Author:
 * Create:
 */
#include "connectivity_log.h"
#include "log_common.h"
#include "ipc.h"
#include "ipc_porting.h"
#include "securec.h"
#include "soc_osal.h"

static system_config_t *g_system_config_p = (system_config_t *)(uintptr_t)(SYSTEM_CFG_REGION_START);

#if CORE == APPS
static bool g_log_level_inited = false;
static log_level_config_t g_log_level_config = {
    LOG_LEVEL_APP_DEFAULT_CONFIG,
    LOG_LEVEL_BT_DEFAULT_CONFIG,
    LOG_LEVEL_DSP_DEFAULT_CONFIG,
};

static log_level_e get_log_level_from_sharemem(uint8_t data_index)
{
    if (g_system_config_p == NULL) {
        return (log_level_e)DEFAULT_LOG_LEVEL;
    }

    uint32_t mgc_log_lvl = *((uint32_t *)g_system_config_p + data_index);
    if ((get_log_level_magic(mgc_log_lvl) != LOG_LEVEL_MAGIC) ||
        (get_real_log_level(mgc_log_lvl) >= LOG_LEVEL_MAX) ||
        (get_real_log_level(mgc_log_lvl) == LOG_LEVEL_NONE)) {
        /* Set the log level in sharemem when chip power on */
        mgc_log_lvl = set_log_level_magic(*((uint32_t *)(&g_log_level_config) + data_index));
        *((uint32_t *)g_system_config_p + data_index) = mgc_log_lvl;
    }
    return (log_level_e)get_real_log_level(mgc_log_lvl);
}

/* Init the log level system */
void init_log_level(void)
{
    if (g_log_level_inited) {
        return;
    }

    /* Point to the share memory and clear the area */
    g_system_config_p = (system_config_t *)(uintptr_t)(SYSTEM_CFG_REGION_START);  //lint !e834
    log_level_e log_lvl = (log_level_e)DEFAULT_LOG_LEVEL;

    log_level_e temp_lvl;
    for (uint8_t i = 0; i < (sizeof(log_level_config_t) / sizeof(uint32_t)); i++) {
        temp_lvl = get_log_level_from_sharemem(i);
        *((uint32_t *)(&g_log_level_config) + i) = temp_lvl;
        if (i == (offsetof(log_level_config_t, app_core_level) / sizeof(uint32_t))) {
            log_lvl = temp_lvl;
        }
    }

    g_log_level_inited = true;

    log_set_local_log_level(log_lvl);
}

/* Inform another core to set the log level. */
static void inform_set_log_level(cores_t core, log_level_e level)
{
    uint32_t ipc_log_level = level;
    ipc_msg_info_t head = {0};
    head.dst_core = core;
    head.priority = 1;
    head.msg_id = IPC_MSG_SET_LOG_LEVEL;
    head.buf_addr = (uint8_t *)&ipc_log_level;
    head.buf_len = (size_t)sizeof(uint32_t);
    (void)uapi_ipc_send_msg_sync(&head);
}

ret_type_e set_log_mode(cores_t core, log_mode_t mode)
{
    if (core == CORES_PROTOCOL_CORE) {
        g_system_config_p->log_mode.dsp_core_mode = (uint32_t)mode;
        return RET_TYPE_OK;
    } else {
        return RET_TYPE_ERROR;
    }
}

ret_type_e set_log_level(cores_t core, log_level_e level)
{
    /* If the log level isn't inited, init first to set the default log level into the shared memory */
    if (!g_log_level_inited) {
        init_log_level();
    }

    if (level >= LOG_LEVEL_MAX) {
        level = LOG_LEVEL_DBG;
    }

    /* Record the log level if the current core is application core */
    uint32_t irq_status = osal_irq_lock();
    switch (core) {
        case CORES_APPS_CORE:
            g_log_level_config.app_core_level = (uint32_t)level;
            g_system_config_p->log_level.app_core_level = set_log_level_magic((uint32_t)level);
            break;

        case CORES_BT_CORE:
            g_log_level_config.bt_core_level = (uint32_t)level;
            g_system_config_p->log_level.bt_core_level = set_log_level_magic((uint32_t)level);
            break;
        case CORES_PROTOCOL_CORE:
            g_log_level_config.dsp_core_level = (uint32_t)level;
            g_system_config_p->log_level.dsp_core_level = set_log_level_magic((uint32_t)level);
            break;
        default:
            osal_irq_restore(irq_status);
            return RET_TYPE_ERROR;
    }
    osal_irq_restore(irq_status);
    /* Set the log level of low layer */
    if (core == CORES_APPS_CORE) {
        log_set_local_log_level(level);
    } else {
        inform_set_log_level(core, level);
    }

    return RET_TYPE_OK;
}
#endif

ret_type_e get_log_level(cores_t core, log_level_e *level)
{
    if (level == NULL) {
        return RET_TYPE_ERROR;
    }

#if CORE == APPS
    if (!g_log_level_inited) {
        init_log_level();
    }

    if (core == CORES_APPS_CORE) {
        *level = (log_level_e)g_log_level_config.app_core_level;
    } else if (core == CORES_BT_CORE) {
        *level = (log_level_e)g_log_level_config.bt_core_level;
    } else if (core == CORES_PROTOCOL_CORE) {
        *level = (log_level_e)g_log_level_config.dsp_core_level;
    } else {
    }
#else
    unused(core);
    unused(level);
    *level = log_get_local_log_level();
#endif

    return RET_TYPE_OK;
}

ret_type_e get_log_mode(cores_t core, log_mode_t* const mode)
{
    if (mode == NULL) {
        return RET_TYPE_ERROR;
    }

    if (core == CORES_PROTOCOL_CORE) {
        *mode = (log_mode_t)g_system_config_p->log_mode.dsp_core_mode;
        return RET_TYPE_OK;
    } else {
        return RET_TYPE_ERROR;
    }
}

void log_set_sharemem_timestamp(uint64_t rtc_time_ms)
{
    uint32_t irq_status = osal_irq_lock();
    g_system_config_p->log_time = rtc_time_ms;
    osal_irq_restore(irq_status);
}

uint64_t log_get_sharemem_timestamp(void)
{
    return g_system_config_p->log_time;
}

void set_slave_cpu_power_mode(slave_cpu_t slave_cpu, slave_cpu_power_mode_t slave_mode)
{
    if ((slave_cpu >= SLAVE_CPU_MAX_NUM) || (slave_mode >= SLAVE_CPU_POWER_MAX_NUM)) {
        return;
    }

    uint32_t irq_status = osal_irq_lock();
    if (slave_cpu == SLAVE_CPU_DSP0) {
        g_system_config_p->dsp0_power_flag = (uint32_t)slave_mode;
    } else {
        g_system_config_p->bt_power_flag = (uint32_t)slave_mode;
    }
    osal_irq_restore(irq_status);
}

slave_cpu_power_mode_t get_slave_cpu_power_mode(slave_cpu_t slave_cpu)
{
    if (slave_cpu >= SLAVE_CPU_MAX_NUM) {
        return SLAVE_CPU_POWER_MAX_NUM;
    }
    if (slave_cpu == SLAVE_CPU_DSP0) {
        return (slave_cpu_power_mode_t)g_system_config_p->dsp0_power_flag;
    } else {
        return (slave_cpu_power_mode_t)g_system_config_p->bt_power_flag;
    }
}
