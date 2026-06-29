/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description:  massdata reader module.
 */

#include "errcode.h"
#include "securec.h"
#include "non_os.h"
#include "panic.h"
#include "soc_osal.h"
#include "common_def.h"
#include "memory_config.h"
#include "dfx_adapt_layer.h"
#include "massdata_buffer.h"
#include "log_uart.h"
#if (defined (CONFIG_DFX_SUPPORT_DIAG_REPORT) && (CONFIG_DFX_SUPPORT_DIAG_REPORT == DFX_YES) \
    && defined (CONFIG_DFX_CLOUD_REPORT))
#include "diag_log_hook.h"
#endif
#if (CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE == DFX_YES)
#include "log_file.h"
#endif
#include "massdata_trigger.h"
#include "debug_print.h"
#include "massdata_reader.h"

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
#endif

#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES) \
    && (CORE == CORE_LOGGING))

static uint8_t g_massdata_state = 0;

#define MASSDATA_BIT_8 8
#define MASSDATA_EVENT_SIZE sizeof(system_event_s_t)

STATIC void massdata_print(system_event_s_t *event)
{
    unused(event);
    PRINT("system event: [%x] %llu(10ms), %#x, %#x, %#x, %#x data_len = %d\n", (uint8_t)event->head_flag,
        event->time_stamp, event->event_id >> MASSDATA_BIT_8, event->event_id & 0xFF, event->event_info,
        event->psn, event->data_len);
}

/**
 * @brief  Print mass data to uart
 */
STATIC void massdata_reader_uart(uint8_t *b1, uint32_t l1, uint8_t *b2, uint32_t l2)
{
    uint8_t *data = b1;
    uint32_t data_len = l1;
    uint32_t len = 0;
    PRINT("massdata: l1 = %d l2 = %d\r\n", l1, l2);

    if (b2 != NULL && l2 != 0) {
        data_len = l1 + l2;
        data = (uint8_t *)dfx_malloc(0, data_len);
        if (data == NULL) {
            return;
        }
        if (memcpy_s(data, data_len, b1, l1) != EOK) {
            dfx_free(0, data);
            return;
        }
        if (memcpy_s(data + l1, data_len - l1, b2, l2) != EOK) {
            dfx_free(0, data);
            return;
        }
    }

    uint8_t *event_data = data;
    while (len < data_len) {
        system_event_s_t *events = (system_event_s_t *)event_data;
        if (events->head_flag != MASSDATA_HEAD_MAGIC) {
            len++;
            event_data++;
            continue;
        }
        massdata_print(events);
#if (defined (CONFIG_DFX_SUPPORT_DIAG_REPORT) && (CONFIG_DFX_SUPPORT_DIAG_REPORT == DFX_YES) \
    && defined (CONFIG_DFX_CLOUD_REPORT))
        diag_massdata_add(events);
#endif
        len += (MASSDATA_EVENT_SIZE + events->data_len);
        event_data += (MASSDATA_EVENT_SIZE + events->data_len);
    }

    if (b2 != NULL && l2 != 0) {
        dfx_free(0, data);
    }
}

massdata_reader_ret_t massdata_reader_has_next(massdata_memory_region_section_t *mmsec)
{
    *mmsec = MASS_MEMORY_REGION_MAX_NUMBER;

    massdata_memory_region_control_t *mass_ctrl = MASS_MEMORY_REGION_CONTROL_BLOCK_POINTER;
    for (uint8_t i_mmsec = MASS_MEMORY_REGION_SECTION_0; i_mmsec < MASS_MEMORY_REGION_MAX_NUMBER; i_mmsec++) {
        massdata_memory_section_control_t *mass_section_control = &(mass_ctrl->section_control[i_mmsec]);
        if (mass_section_control->write != mass_section_control->read) {
            *mmsec = i_mmsec;
            break;
        }
    }

    if (*mmsec == MASS_MEMORY_REGION_MAX_NUMBER) {
        return MASSDATA_READER_RET_THERE_IS_NO_NEXT_MESSAGE;
    }
    return MASSDATA_READER_RET_OK;
}

massdata_reader_ret_t massdata_reader_get_next(massdata_memory_region_section_t mmsec,
    uint8_t **p1, uint32_t *len1, uint8_t **p2, uint32_t *len2)
{
    if (mmsec >= MASS_MEMORY_REGION_MAX_NUMBER ||
        p1 == NULL || len1 == NULL || p2 == NULL || len2 == NULL) {
        return MASSDATA_READER_RET_ERROR_IN_PARAMS;
    }

    massdata_memory_region_control_t *mass_ctrl = MASS_MEMORY_REGION_CONTROL_BLOCK_POINTER;
    massdata_memory_section_control_t *mass_section_control = &(mass_ctrl->section_control[mmsec]);

    uint32_t read = mass_section_control->read;
    uint32_t write = mass_section_control->write;
    uint32_t size = mass_section_control->region_len;

    if (read <= write) {
        *p1 = (uint8_t *)((uintptr_t)(mass_section_control->region_start + read));
        *len1 = write - read;
        *p2 = NULL;
        *len2 = 0;
    } else {
        *p1 = (uint8_t *)((uintptr_t)(mass_section_control->region_start + read));
        *len1 = size - read;
        *p2 = (uint8_t *)((uintptr_t)mass_section_control->region_start);
        *len2 = write;
    }

    if (*len1 + *len2 > size) {
        return MASSDATA_READER_RET_ERROR_OVERFLOW_ON_DISCARDING;
    }

    return MASSDATA_READER_RET_OK;
}

massdata_reader_ret_t massdata_reader_discard(massdata_memory_region_section_t mmsec, uint32_t len)
{
    if (mmsec >= MASS_MEMORY_REGION_MAX_NUMBER) {
        return MASSDATA_READER_RET_ERROR_IN_PARAMS;
    }

    massdata_memory_region_control_t *mass_ctrl = MASS_MEMORY_REGION_CONTROL_BLOCK_POINTER;
    massdata_memory_section_control_t *mass_section_control = &(mass_ctrl->section_control[mmsec]);

    uint32_t read = mass_section_control->read;
    uint32_t write = mass_section_control->write;
    uint32_t size = mass_section_control->region_len;

    if (read == write) {
        return MASSDATA_READER_RET_ERROR_TRYING_TO_DISCARD_WHEN_EMPTY;
    }

    read = (read + len) % size;

    if (read > write) {
        read = write;
        mass_section_control->read = read;
        return MASSDATA_READER_RET_ERROR_CORRUPT_SHARED_MEMORY;
    }

    mass_section_control->read = read;
    return MASSDATA_READER_RET_OK;
}

#if ((CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE == DFX_YES))
STATIC uint32_t massdata_check_record_valid(uint8_t *data, uint32_t data_len)
{
    if (data == NULL || data_len < sizeof(system_event_s_t)) {
        return 0;
    }

    system_event_s_t *events = (system_event_s_t *)data;
    if (events->head_flag == MASSDATA_HEAD_MAGIC && (events->data_len + sizeof(system_event_s_t) < data_len)) {
        if (data[events->data_len + sizeof(system_event_s_t)] == MASSDATA_HEAD_MAGIC) {
            return events->data_len + sizeof(system_event_s_t);
        }
    }
    return 0;
}

/**
 * @brief  Write mass data to file
 */
STATIC void massdata_reader_file(uint8_t *b1, uint32_t l1, uint8_t *b2, uint32_t l2)
{
    PRINT("massdata: l1 = %d l2 = %d\r\n", l1, l2);
    errcode_t ret = uapi_logfile_write(STORE_DOTTING, 0, b1, l1);
    if (ret == ERRCODE_SUCC && b2 != NULL) {
        ret = uapi_logfile_write(STORE_DOTTING, 0, b2, l2);
    }
    if (ret != ERRCODE_SUCC) {
        PRINT("error write mass data file, ret=0x%x\r\n", ret);
    }
}

#endif /* CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE */

#endif /* CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE */

void uapi_massdata_set_state(uint8_t state)
{
    unused(state);

#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES) \
    && (CORE == CORE_LOGGING))
    if (g_massdata_state == state) {
        return;
    }

    if (state == MASS_ENABLED_FILE) {
#if ((CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE == DFX_YES))
        errcode_t ret;

        store_file_cfg_t cfg;
        cfg.mult_files = MASSDATA_MUTI_FILE_NUM;
        cfg.cache_size = MASSDATA_CACHE_SIZE;
        cfg.enable_cache = MASSDATA_CACHE_ENABLE;
        cfg.file_size = MASSDATA_FILE_SIZE;
        cfg.path = MASSDATA_FILE_PATH;
        cfg.name = MASSDATA_FILE_NAME;
        cfg.no_record_head = 1;
        cfg.record_max_size = MASSDATA_RECORD_MAX_SIZE;
        cfg.callback = massdata_check_record_valid;
        ret = uapi_logfile_open(STORE_DOTTING, &cfg);
        if (ret != ERRCODE_SUCC && ret != ERRCODE_DFX_LOGFILE_ALREADY_OPEN) {
            PRINT("open logfile %s failed. ret = 0x%x\r\n", cfg.name, ret);
            return;
        }

        massdata_register_reader_callback(massdata_reader_file);
#endif
    } else {
        if (g_massdata_state == MASS_ENABLED_FILE) {
#if ((CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE == DFX_YES))
            (void)uapi_logfile_close(STORE_DOTTING);
#endif
        }
        if (state == MASS_ENABLED_UART) {
            massdata_register_reader_callback(massdata_reader_uart);
        } else {
            massdata_register_reader_callback(NULL);
        }
    }
    g_massdata_state = state;
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE */
}

