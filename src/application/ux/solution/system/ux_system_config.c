/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux system config service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-07-23， Create file. \n
 */
#include <securec.h>
#include <stdio.h>
#include <string.h>
#include "ux_hal_memory.h"
#include "ux_common_log.h"
#include "ux_errcode.h"

#ifndef DEMO_WATCH
#include "ux_device_manager.h"
#include "ux_system_config.h"
#if defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_BMIC_TX)
#include "key_struct.h"
#include "key_id.h"
#include "property.h"
#include "hal_pmu_ulp.h"
#endif
#else
#include "ux_api_system_config.h"
#include "build_version.h"
#endif

#ifndef DEMO_WATCH
errcode_t ux_system_cfg_set_sdk_version(uint16_t device_id, char* sdk_version, uint16_t data_len)
{
    ux_system_settings_table_t *system_table = ux_device_mgr_get_system_settings_table(device_id);
    if (system_table == NULL) {
        ux_log_err("[ux][system]CFG table for device %u not found\n", device_id);
        return UX_ERR_DEVICE_MGR_TABLE_NOT_FOUND;
    }

    if (system_table->sdk_version != NULL) {
        ux_hal_memory_release(UX_MEM_CMD_SYS, system_table->sdk_version);
        system_table->sdk_version = NULL;
    }

    system_table->sdk_version = ux_hal_memory_acquire(UX_MEM_CMD_SYS, data_len);
    if (system_table->sdk_version == NULL) {
        ux_log_err("[ux][system]malloc memory failed\n");
        return UX_ERR_MALLOC_FAILED;
    }

    system_table->sdk_version_len = (uint8_t)data_len;
    errno_t err = memcpy_s(system_table->sdk_version, data_len, sdk_version, data_len);
    if (err != EOK) {
        ux_log_err("[ux][system][ux_system_cfg_set_sdk_version] memcpy_s failed");
        return ERRCODE_MEMCPY;
    }

    return UX_ERR_SUCCESS;
}

errcode_t ux_system_cfg_get_sdk_version(uint16_t device_id, char* sdk_version, uint16_t data_len)
{
    if (sdk_version == NULL) {
        ux_log_err("[ux][system]Invalid record parameters\n");
        return UX_ERR_INVALID_PARAMETER;
    }

    ux_system_settings_table_t *system_table = ux_device_mgr_get_system_settings_table(device_id);
    if (system_table == NULL || system_table->sdk_version == NULL) {
        ux_log_err("[ux][system]CFG table or sdk_version for device %u not found\n", device_id);
        return UX_ERR_DEVICE_MGR_TABLE_NOT_FOUND;
    }

    errno_t err = memcpy_s(sdk_version, data_len, system_table->sdk_version, system_table->sdk_version_len);
    if (err != EOK) {
        ux_log_err("[ux][system][ux_system_cfg_get_sdk_version] memcpy_s failed");
        return ERRCODE_MEMCPY;
    }
    return UX_ERR_SUCCESS;
}
#else /* for 3322 ux */
/* solution提供的专用于获取sdk version的接口 */
errcode_t ux_api_get_sdk_version(char *sdk_version, uint8_t sdk_version_len)
{
    if (sdk_version == NULL || sdk_version_len == 0) {
        ux_log_err("[ux][system]ux_api_get_sdk_version params invalid\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }

    errno_t ret = strcpy_s(sdk_version, sdk_version_len, get_version_string());
    if (ret != EOK) {
        ux_log_err("[ux][system]ux_api_get_sdk_version strcpy fail ret = %d\r\n", ret);
        return UX_ERR_STRCPY_FAILED;
    }

    return UX_ERR_SUCCESS;
}

/* solution提供的用于批量获取系统各部件version的接口 */
errcode_t ux_api_get_all_version(ux_api_version_info_t *version_info, uint32_t version_info_size)
{
    /* 参数合法性校验 */
    if (version_info == NULL || version_info_size < sizeof(ux_api_version_info_t)) {
        ux_log_err("[ux][system]ux_api_get_all_version params invalid\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }

    /* 初始化结构体，清空字段避免脏数据 */
    (void)memset_s(version_info, sizeof(ux_api_version_info_t), 0, sizeof(ux_api_version_info_t));

    /* 安全读取sdk版本号信息 */
    errno_t ret = strcpy_s(version_info->sdk_version, sizeof(version_info->sdk_version), get_version_string());
    if (ret != EOK) {
        ux_log_err("[ux][system]ux_api_get_all_version strcpy fail ret = %d\r\n", ret);
        return UX_ERR_STRCPY_FAILED;
    }
    version_info->version_mask |= VERSION_MASK_SDK; /* 当前仅标记SDK版本查询有效 */

    return UX_ERR_SUCCESS;
}
#endif

#if defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_BMIC_TX)
errcode_t ux_sys_set_time(calendar_t calendar)
{
    errcode_t ret = ERRCODE_SUCC;
    timestamp_nv_t timestamp_nv_info;
    ret = uapi_calendar_set_datetime(&calendar);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][sys][ux_sys_set_time]set datatime failed with errorcode 0x%X\r\n", ret);
        return ret;
    }

    ret = uapi_calendar_get_timestamp(&timestamp_nv_info.last_timestamp_ms);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][sys][ux_sys_set_time] get timestamp failed with errorcode 0x%X\r\n", ret);
        return ret;
    }

    timestamp_nv_info.last_systick = hal_pmu_ulp_get_systick_curr_count();
    ret = property_set_bytes(NV_ID_TIMESTAMP_PARA, (uint8_t *)(&timestamp_nv_info), sizeof(timestamp_nv_info));
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][sys][ux_sys_set_time] property_set_bytes failed with errorcode 0x%X\r\n", ret);
    }
    return ret;
}
#endif
