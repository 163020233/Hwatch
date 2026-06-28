/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description:  MASSDATA TRIGGER MODULE
 */

#include "massdata_trigger.h"
#include "massdata_buffer.h"
#include "dfx_adapt_layer.h"

#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_DOTTING) && (CONFIG_DFX_SUPPORT_MASSDATA_DOTTING == DFX_YES))

#if ((CORE == CORE_LOGGING) && defined(BUILD_APPLICATION_STANDARD)) || (CORE_NUMS  == 1)

static massdata_trigger_callback_t g_massdata_trigger_callback = NULL;

void massdata_trigger(void *pay_i, uint8_t core, uint8_t type)
{
    if (g_massdata_trigger_callback != NULL) {
        g_massdata_trigger_callback(pay_i, core, type);
    }
}

void register_massdata_trigger(massdata_trigger_callback_t callback)
{
    if (callback != NULL) {
        g_massdata_trigger_callback = callback;
    }
}

#else  // (CORE != CORE_LOGGING) case

#include "ipc.h"
#ifdef IPC_NEW
#include "ipc_porting.h"
#endif

// Have a definition for the right cores_t enum in CORES_CORE_LOGGING
#if CORE_LOGGING == BT
#define CORES_CORE_LOGGING CORES_BT_CORE
#elif CORE_LOGGING == PROTOCOL
#define CORES_CORE_LOGGING CORES_PROTOCOL_CORE
#elif CORE_LOGGING == APPS
#define CORES_CORE_LOGGING CORES_APPS_CORE
#elif CORE_LOGGING == GNSS
#define CORES_CORE_LOGGING CORES_GNSS_CORE
#endif

/**
 * @brief  trigger ipc to inform A core to save data from share mem in flash
 */
void massdata_trigger(void *pay_i, uint8_t core, uint8_t type)
{
#if ((CORE != APPS) && defined(SUPPORT_IPC))
    ipc_payload_mass_data_type ipc_pay;
    system_event_s_t *pay = (system_event_s_t *)(pay_i);
    ipc_pay.core = core;
    ipc_pay.type = type;
    ipc_pay.event_id = pay->event_id;
    ipc_pay.time_stamp = pay->time_stamp;
    ipc_pay.event_info = pay->event_info;
    ipc_pay.chr_up_type = pay->chr_up_type;
    ipc_pay.psn = pay->psn;
    ipc_pay.time_not_sync_flag = pay->time_not_sync_flag;
    ipc_pay.role = pay->role;
#ifdef SUPPORT_IPC
    (void)ipc_send_message(CORES_APPS_CORE,
                           IPC_ACTION_MASS_DATA_INFORM,
                           (ipc_payload *)((void *)&ipc_pay),
                           sizeof(ipc_payload_mass_data_type),
                           IPC_PRIORITY_LOWEST, false);
#elif defined(IPC_NEW)
    ipc_msg_info_t msg_info;
    msg_info.dst_core = CORES_APPS_CORE;
    msg_info.priority = 1;
    msg_info.channel = 0;
    msg_info.msg_id = IPC_MSG_MASS_DATA_INFORM;
    msg_info.buf_addr = (uint8_t *)&ipc_pay;
    msg_info.buf_len = sizeof(ipc_payload_mass_data_type);
    uapi_ipc_send_msg_async(&msg_info);
#endif
#else
    unused(pay_i);
    unused(core);
    unused(type);
#endif
}

#endif /* (CORE == CORE_LOGGING) */
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_DOTTING */
