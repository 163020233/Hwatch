/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description:  massdata buffer module.
 */

#include "securec.h"
#include "non_os.h"
#include "panic.h"
#include "soc_osal.h"
#include "common_def.h"
#include "memory_config.h"
#include "dfx_adapt_layer.h"
#include "massdata_trigger.h"
#include "log_trigger.h"
#include "systick.h"
#include "log_printf.h"
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT) && (CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT == DFX_YES))
#include "ipc.h"
#if defined(SUPPORT_IPC)
#include "ipc_actions.h"
#elif defined(IPC_NEW)
#include "ipc_porting.h"
#endif
#endif
#include "massdata_buffer.h"

#define CONTOL_BLOCK_MASS_LENGTH        sizeof(massdata_memory_region_control_t)
/* Offsets from the start of the memory region */
#define BT_CORE_MASS_OFFSET             (MASSDATA_REGION_START + CONTOL_BLOCK_MASS_LENGTH)
#define APPLICATION_CORE_MASS_OFFSET    (BT_CORE_MASS_OFFSET + BT_MASSDATA_LENGTH)

#if CORE == BT
#define MASS_MEMORY_REGION_SECTION_CORE MASS_MEMORY_REGION_SECTION_0
#elif CORE == APPS
#if CORE_NUMS == 1
#define MASS_MEMORY_REGION_SECTION_CORE MASS_MEMORY_REGION_SECTION_0
#else
#define MASS_MEMORY_REGION_SECTION_CORE MASS_MEMORY_REGION_SECTION_1
#endif
#elif CORE == GNSS
#define MASS_MEMORY_REGION_SECTION_CORE MASS_MEMORY_REGION_SECTION_0
#elif CORE == SECURITY
#define MASS_MEMORY_REGION_SECTION_CORE MASS_MEMORY_REGION_SECTION_0
#elif CORE == CONTROL_CORE
#define MASS_MEMORY_REGION_SECTION_CORE MASS_MEMORY_REGION_SECTION_0
#elif CORE == SENSOR
#define MASS_MEMORY_REGION_SECTION_CORE MASS_MEMORY_REGION_SECTION_0
#elif CORE == CCORE
#define MASS_MEMORY_REGION_SECTION_CORE MASS_MEMORY_REGION_SECTION_0
#endif

#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_DOTTING) && (CONFIG_DFX_SUPPORT_MASSDATA_DOTTING == DFX_YES))

STATIC volatile massdata_memory_section_control_t *g_mass_section_control = NULL;
STATIC uint32_t g_mass_share_mem_size;
STATIC uint32_t g_mass_share_mem_start;
STATIC uint8_t g_role_state;
STATIC uint32_t g_mass_write_ue_psn = 0;
STATIC uint32_t g_mass_write_dft_psn = 0;
STATIC uint32_t g_version_number = 0;
STATIC bool g_mass_time_not_sync_flag = true;

#if defined(SUPPORT_IPC)
#include "ipc.h"
STATIC bool mass_receive_remote_info(ipc_action_t message, const volatile ipc_payload *payload,
                                     cores_t src, uint32_t id)
{
    unused(message);
    unused(src);
    unused(id);

    if (payload != NULL) {
        g_version_number = *(uint32_t *)(uintptr_t)payload;
    }
    return true;
}
#endif /* defined(SUPPORT_IPC) */

#if defined(IPC_NEW)
#include "ipc.h"
#include "ipc_new_shared.h"
STATIC void mass_receive_remote_info(uint8_t *payload, uint32_t payload_len)
{
    unused(payload_len);
    if (payload != NULL) {
        g_version_number = *(uint32_t *)(uintptr_t)payload;
    }
    return;
}
#endif /* defined(SUPPORT_IPC) */

/* eventEnum : (info0 << 16) | (info1 << 8) | info2 */
STATIC uint32_t massdata_get_info0_value(uint32_t event_enum)
{
    return ((event_enum >> CHR_INFO_SHFIT_16BIT) & 0xFF);
}

STATIC uint16_t massdata_get_event_id_value(uint32_t event_enum)
{
    return (uint16_t)((event_enum >> CHR_INFO_SHFIT_8BIT) & 0xFFFF);
}

STATIC uint8_t massdata_get_info2_value(uint32_t event_enum)
{
    return (uint8_t)(event_enum & 0xFF);
}

/* write mass data to share mem roll buffer, index by core type */
STATIC uint32_t massdata_write_roll_buffer_by_step(massdata_memory_section_control_t *buffer_info,
    const uint8_t *data, uint32_t length)
{
    uint32_t remain_size;
    uint32_t to_end;

    /* check the message length is as much the buffer size */
    if ((data == NULL) || (buffer_info == NULL)) {
        return MASS_RET_ERROR_IN_PARAMETERS;
    }

    uint32_t read = buffer_info->read;
    uint32_t write = buffer_info->write;
    uint32_t size = buffer_info->region_len;
    uint8_t *start = (uint8_t *)(uintptr_t)buffer_info->region_start;

    if ((size == 0) || (write >= size) || (read >= size)) {
        return MASS_RET_ERROR_IN_PARAMETERS;
    }

    to_end = size - write;
    remain_size = (read <= write) ? (size - (write - read)) : (read - write);

    if (remain_size < length) {
        return MASS_RET_ERROR_NOT_ENOUGH_SPACE;
    }

    if (length <= to_end) {
        if (memcpy_s((void *)((uintptr_t)(start + write)), to_end, data, length) != EOK) {
            return MASS_MEM_COPY_FAIL;
        }
    } else {
        if (memcpy_s((void *)((uintptr_t)(start + write)), to_end, data, to_end) != EOK) {
            return MASS_MEM_COPY_FAIL;
        }
        if (memcpy_s((void *)(uintptr_t)start, length - to_end, data + to_end, length - to_end) != EOK) {
            return MASS_MEM_COPY_FAIL;
        }
    }

    buffer_info->write = (write + length) % size;
    return MASS_RET_OK;
}

/**
 * @brief  write seg in the mass share mem.
 * @return MASS_RET_OK or an error value
 */
STATIC uint32_t massdata_write_roll_buffer(const uint8_t *head, uint32_t head_len,
    const uint8_t *data, uint32_t data_len)
{
    uint32_t remain_size;
    uint32_t ret;
    massdata_memory_section_control_t buffer_info = {0};

    /* check the message length is as much the buffer size */
    if ((head == NULL) || (data == NULL) || (g_mass_section_control == NULL)) {
        return MASS_RET_ERROR_IN_PARAMETERS;
    }

    uint32_t irq = osal_irq_lock();

    /* check if mem overflow */
    if ((g_mass_section_control->read > g_mass_share_mem_size) ||
        (g_mass_section_control->write > g_mass_share_mem_size) ||
        (g_mass_section_control->region_len > g_mass_share_mem_size) ||
        (g_mass_section_control->region_start != g_mass_share_mem_start)) {
        /* recorvy mem offset */
        g_mass_section_control->read = 0;
        g_mass_section_control->write = 0;
        g_mass_section_control->region_start = g_mass_share_mem_start;
        g_mass_section_control->region_len = g_mass_share_mem_size;
        g_mass_section_control->water_line = g_mass_share_mem_size / CHR_BUFFER_WL_RATIO;
        osal_irq_restore(irq);
        return MASS_RET_ERROR_CORRUPT_SHARED_MEMORY;
    }

    (void)memcpy_s((void *)&buffer_info, sizeof(buffer_info), (void *)g_mass_section_control, sizeof(buffer_info));

    if (buffer_info.read <= buffer_info.write) {
        remain_size = buffer_info.region_len - (buffer_info.write - buffer_info.read);
    } else {
        remain_size = buffer_info.read - buffer_info.write;
    }

    if (remain_size < (head_len + data_len)) {
        ret = MASS_RET_ERROR_NOT_ENOUGH_SPACE;
        goto err;
    }

    ret = massdata_write_roll_buffer_by_step(&buffer_info, head, head_len);
    if (ret != MASS_RET_OK) {
        goto err;
    }

    ret = massdata_write_roll_buffer_by_step(&buffer_info, data, data_len);
    if (ret != MASS_RET_OK) {
        goto err;
    }

    g_mass_section_control->write = buffer_info.write;
    osal_irq_restore(irq);

    remain_size -= (head_len + data_len);

    if (remain_size < buffer_info.water_line) { return MASS_OVER_BUFFER_THD; }
    return MASS_RET_OK;
err:
    osal_irq_restore(irq);
    return ret;
}
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_DOTTING */

void uapi_massdata_record_ue_event(uint8_t event_id, uint8_t info1, uint8_t info2, uint32_t info3)
{
    uint32_t event_enum = ((event_id << CHR_INFO_SHFIT_16BIT) | (info1 << CHR_INFO_SHFIT_8BIT) | info2);
    return uapi_massdata_record_long_event(event_enum, (uint8_t*)&info3, sizeof(uint32_t));
}

void uapi_massdata_record_system_error(uint8_t event_id, uint8_t info1, uint8_t info2, uint32_t info3)
{
    uint32_t event_enum = ((event_id << CHR_INFO_SHFIT_16BIT) | (info1 << CHR_INFO_SHFIT_8BIT) | info2);
    return uapi_massdata_record_long_event(event_enum, (uint8_t*)&info3, sizeof(uint32_t));
}

void uapi_massdata_record_system_event(uint8_t event_id, uint8_t info1, uint8_t info2, uint32_t info3)
{
    uint32_t event_enum = ((event_id << CHR_INFO_SHFIT_16BIT) | (info1 << CHR_INFO_SHFIT_8BIT) | info2);
    return uapi_massdata_record_long_event(event_enum, (uint8_t*)&info3, sizeof(uint32_t));
}

void uapi_massdata_record_system_long_event(uint8_t event_id, uint8_t info1, uint8_t info2,
                                            uint8_t *info3, uint8_t info3_len)
{
    uint32_t event_enum = ((event_id << CHR_INFO_SHFIT_16BIT) | (info1 << CHR_INFO_SHFIT_8BIT) | info2);
    return uapi_massdata_record_long_event(event_enum, info3, info3_len);
}

void uapi_massdata_buffer_init(massdata_memory_region_section_t sec)
{
    unused(sec);
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_DOTTING) && (CONFIG_DFX_SUPPORT_MASSDATA_DOTTING == DFX_YES))
#if CORE_NUMS > 1
    if (sec != MASS_MEMORY_REGION_SECTION_0 && sec != MASS_MEMORY_REGION_SECTION_1) {
        return;
    }
#else
    if (sec != MASS_MEMORY_REGION_SECTION_0) {
        return;
    }
#endif /* CORE_NUMS */
    massdata_memory_region_control_t *mass_ctrl = MASS_MEMORY_REGION_CONTROL_BLOCK_POINTER;
    g_mass_section_control = &(mass_ctrl->section_control[sec]);

    g_mass_share_mem_start = g_mass_section_control->region_start;
    g_mass_share_mem_size = g_mass_section_control->region_len;
    g_mass_section_control->water_line = g_mass_share_mem_size / CHR_BUFFER_WL_RATIO;

#ifdef SUPPORT_IPC
    ipc_register_handler(IPC_ACTION_MASS_DATA_INFORM, mass_receive_remote_info);
#elif defined(IPC_NEW)
    ipc_rx_handler_info_t handler_info = {0};
    handler_info.msg_id = IPC_MSG_MASS_DATA_INFORM;
    handler_info.cb = mass_receive_remote_info;
    uapi_ipc_register_rx_handler(&handler_info);
#endif
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_DOTTING */
}

void uapi_massdata_memory_region_init(void)
{
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_DOTTING) && (CONFIG_DFX_SUPPORT_MASSDATA_DOTTING == DFX_YES))
    memset_s((void *)(uintptr_t)MASSDATA_REGION_START, MASSDATA_REGION_LENGTH, 0, MASSDATA_REGION_LENGTH);
    massdata_memory_region_control_t *mass_ctrl = MASS_MEMORY_REGION_CONTROL_BLOCK_POINTER;

    mass_ctrl->region_num = MASS_MEMORY_REGION_MAX_NUMBER;
    mass_ctrl->mem_len = MASSDATA_REGION_LENGTH;
    mass_ctrl->section_control[MASS_MEMORY_REGION_SECTION_0].region_start = (uint32_t)BT_CORE_MASS_OFFSET;
    mass_ctrl->section_control[MASS_MEMORY_REGION_SECTION_0].region_len = BT_MASSDATA_LENGTH;
#if CORE_NUMS > 1
    mass_ctrl->section_control[MASS_MEMORY_REGION_SECTION_1].region_start = (uint32_t)APPLICATION_CORE_MASS_OFFSET;
    mass_ctrl->section_control[MASS_MEMORY_REGION_SECTION_1].region_len = APP_MASSDATA_LENGTH -
                                                                          (uint32_t)CONTOL_BLOCK_MASS_LENGTH;
#endif /* CORE_NUMS > 1 */
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_DOTTING */
    return;
}

void uapi_massdata_set_role(uint8_t role)
{
    unused(role);
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_DOTTING) && (CONFIG_DFX_SUPPORT_MASSDATA_DOTTING == DFX_YES))
    g_role_state = role;
#endif
}

void uapi_massdata_record_long_event(uint32_t event_enum, uint8_t *info3, uint8_t info3_len)
{
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_DOTTING) && (CONFIG_DFX_SUPPORT_MASSDATA_DOTTING == DFX_YES))
    uint32_t ret;
    uint32_t psn;
    uint8_t chr_type = CHR_UE_JSON;
    uint32_t irq;

    if (info3 == NULL || info3_len > MAX_INFO3_DATA_SIZE) {
        return;
    }

    if (massdata_get_info0_value(event_enum) >= EVENT_DFT_START) {
        chr_type = CHR_DFT;
        irq = osal_irq_lock();
        psn = g_mass_write_dft_psn;
        g_mass_write_dft_psn = (g_mass_write_dft_psn + 1) % (1 << CHR_PSN_BIT);
        osal_irq_restore(irq);
    } else {
        irq = osal_irq_lock();
        psn = g_mass_write_ue_psn;
        g_mass_write_ue_psn = (g_mass_write_ue_psn + 1) % (1 << CHR_PSN_BIT);
        osal_irq_restore(irq);
    }

    /* fill the mass packet header */
    system_event_s_t event_seg;
    event_seg.head_flag = MASSDATA_HEAD_MAGIC;
    event_seg.time_stamp = massdata_adapt_get_time();
    event_seg.data_len = info3_len;
    event_seg.event_id = massdata_get_event_id_value(event_enum);
    event_seg.event_info = massdata_get_info2_value(event_enum);
    event_seg.chr_up_type = chr_type;
    event_seg.version = (uint16_t)g_version_number;
    event_seg.core = CORE;
    event_seg.reserved = 0;
    event_seg.psn = psn;
    event_seg.time_not_sync_flag = g_mass_time_not_sync_flag;
    event_seg.role = g_role_state;

    ret = massdata_write_roll_buffer((const uint8_t*)&event_seg, sizeof(system_event_s_t), info3, info3_len);
    if ((ret == MASS_OVER_BUFFER_THD || ret == MASS_RET_ERROR_NOT_ENOUGH_SPACE)) {
        massdata_trigger((void *)&event_seg, CORE, MASS_EVENT_POINT);
    }
#else /* CONFIG_DFX_SUPPORT_MASSDATA_DOTTING */
    unused(event_enum);
    unused(info3);
    unused(info3_len);
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_DOTTING */
    return;
}

void log_buffer_record_system_event(uint32_t chr_event)
{
    if (chr_event == CHR_VALUE_NULL) {
        return;
    }

    uint8_t event_id;
    uint8_t info1;
    uint8_t info2;
    uint8_t info3;

    event_id = (chr_event >> CHR_INFO_SHFIT_24BIT) & CHR_VALUE_MASK;
    info1 = (chr_event >> CHR_INFO_SHFIT_16BIT) & CHR_VALUE_MASK;
    info2 = (chr_event >> CHR_INFO_SHFIT_8BIT) & CHR_VALUE_MASK;
    info3 = chr_event & CHR_VALUE_MASK;

    uapi_massdata_record_system_event(event_id, info1, info2, info3);
}

void log_buffer_record_system_error(uint32_t chr_error)
{
    if (chr_error == CHR_VALUE_NULL) {
        return;
    }

    uint8_t error_id;
    uint8_t info1;
    uint8_t info2;
    uint8_t info3;

    error_id = (chr_error >> CHR_INFO_SHFIT_24BIT) & CHR_VALUE_MASK;
    info1 = (chr_error >> CHR_INFO_SHFIT_16BIT) & CHR_VALUE_MASK;
    info2 = (chr_error >> CHR_INFO_SHFIT_8BIT) & CHR_VALUE_MASK;
    info3 = chr_error & CHR_VALUE_MASK;

    uapi_massdata_record_system_event(error_id, info1, info2, info3);
}

void uapi_massdata_init(void)
{
    uapi_massdata_memory_region_init();
    return uapi_massdata_buffer_init(MASS_MEMORY_REGION_SECTION_CORE);
}

__attribute__((weak)) uint64_t massdata_adapt_get_time(void)
{
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_DOTTING) && (CONFIG_DFX_SUPPORT_MASSDATA_DOTTING == DFX_YES))
    if (massdata_get_time_not_sync_flag()) {
        return uapi_systick_get_ms() / 10; /* 10:convert ms to 10ms units */
    }
#endif
    return uapi_systick_get_ms() / 10 + get_log_basetime_10ms(); /* 10:convert ms to 10ms units */
}

#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT) && (CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT == DFX_YES))
static void massdata_record_system_send_by_ipc(uint32_t msg_id, uint8_t *data_addr, uint32_t data_len)
{
#ifdef SUPPORT_IPC
    (void)ipc_send_message(CORES_APPS_CORE,
                           (ipc_action_t)msg_id,
                           (ipc_payload *)((void *)data_addr),
                           data_len,
                           IPC_PRIORITY_LOWEST, false);
#elif defined(IPC_NEW)
    ipc_msg_info_t msg_info;
    msg_info.dst_core = CORES_APPS_CORE;
    msg_info.priority = 1;
    msg_info.channel = 0;
    msg_info.msg_id = (uint8_t)msg_id;
    msg_info.buf_addr = (uint8_t *)data_addr;
    msg_info.buf_len = data_len;
    uapi_ipc_send_msg_async(&msg_info);
#endif
}
#endif

/* 暂时兼容旧接口 */
void massdata_record_system_error(uint8_t event_id, uint8_t info1, uint8_t info2, uint8_t info3)
{
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT) && (CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT == DFX_YES))
    ipc_payload_mass_data_type chr_info;
    chr_info.type = MASS_ERROR_POINT;
    chr_info.event_id = event_id;
    chr_info.info1 = info1;
    chr_info.info2 = info2;
    chr_info.info3 = info3;

#ifdef SUPPORT_IPC
    massdata_record_system_send_by_ipc(IPC_ACTION_MASS_DATA_INFORM,
        (uint8_t *)&chr_info, sizeof(ipc_payload_mass_data_type));
#elif defined(IPC_NEW)
    massdata_record_system_send_by_ipc(IPC_MSG_MASS_DATA_INFORM,
        (uint8_t *)&chr_info, sizeof(ipc_payload_mass_data_type));
#endif
#else /* CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT */
    return uapi_massdata_record_system_event(event_id, info1, info2, info3);
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT */
}

void massdata_record_system_event(uint8_t event_id, uint8_t info1, uint8_t info2, uint8_t info3)
{
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT) && (CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT == DFX_YES))
    ipc_payload_mass_data_type chr_info;
    chr_info.type = MASS_EVENT_POINT;
    chr_info.event_id = event_id;
    chr_info.info1 = info1;
    chr_info.info2 = info2;
    chr_info.info3 = info3;

#ifdef SUPPORT_IPC
    massdata_record_system_send_by_ipc(IPC_ACTION_MASS_DATA_INFORM,
        (uint8_t *)&chr_info, sizeof(ipc_payload_mass_data_type));
#elif defined(IPC_NEW)
    massdata_record_system_send_by_ipc(IPC_MSG_MASS_DATA_INFORM,
        (uint8_t *)&chr_info, sizeof(ipc_payload_mass_data_type));
#endif
#else /* CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT */
    return uapi_massdata_record_system_event(event_id, info1, info2, info3);
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT */
}

void massdata_record_system_info_with_extend(chr_extend_data *extend_data)
{
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT) && (CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT == DFX_YES))
    if ((extend_data == NULL) ||
        (extend_data->type != MASS_EVENT_POINT_EXTEND && extend_data->type != MASS_ERROR_POINT_EXTEND)) {
        return;
    }
    ipc_payload_mass_data_extend_type chr_data = { 0 };
    chr_data.mass_data.type = extend_data->type;
    chr_data.mass_data.event_id = extend_data->event_id;
    chr_data.mass_data.info1 = extend_data->info1;
    chr_data.mass_data.info2 = extend_data->info2;
    chr_data.mass_data.info3 = extend_data->info3;
    chr_data.data_len = extend_data->data_len;
    if (memcpy_s(chr_data.data, CHR_EXTEND_PARAM_MAX_LEN, extend_data->data, extend_data->data_len) != 0) {
        return;
    }

#ifdef SUPPORT_IPC
    massdata_record_system_send_by_ipc(IPC_ACTION_MASS_DATA_INFORM_WITH_EXTEND,
        (uint8_t *)&chr_data, sizeof(ipc_payload_mass_data_extend_type));
#elif defined(IPC_NEW)
    massdata_record_system_send_by_ipc(IPC_MSG_MASS_DATA_INFORM_WITH_EXTEND,
        (uint8_t *)&chr_data, sizeof(ipc_payload_mass_data_extend_type));
#endif

#else /* CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT */
    unused(extend_data);
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_OLD_FORMAT */
}

#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_DOTTING) && (CONFIG_DFX_SUPPORT_MASSDATA_DOTTING == DFX_YES))
bool massdata_get_time_not_sync_flag(void)
{
    return g_mass_time_not_sync_flag;
}

void massdata_set_time_not_sync_flag(bool flag)
{
    g_mass_time_not_sync_flag = flag;
}
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_DOTTING */
