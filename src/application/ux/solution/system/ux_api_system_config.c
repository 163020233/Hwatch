/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux system config service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-08-23， Create file. \n
 */
#include "common_def.h"
#ifndef DEMO_WATCH
#include "stddef.h"
#include "feature_config.h"
#include "ux_audio_effect.h"
#ifdef CONFIG_SUPPORT_FILESYSTEM
#include "mmc_porting.h"
#include "errno.h"
#endif
#include "ux_usb.h"
#ifdef CONFIG_SUPPORT_AUDIO_RECORDER
#include "ux_audio_recorder.h"
#endif
#include "osal_task.h"
#include "ux_common_log.h"
#ifdef XTS_SUPPORT
#include "key_id.h"
#include "power_manager.h"
#endif
#include "property.h"
#include "key_id.h"
#include "audio_manager_service.h"
#include "ux_api_fwk.h"
#include "ux_audio_stream.h"
#include "ux_audio_recorder.h"
#include "ux_bs_sle.h"
#include "ux_bs_rpc.h"
#include "ux_errcode.h"
#include "ux_event_define.h"
#include "ux_system_config.h"
#include "ux_config_common.h"
#include "ux_api_system_config.h"

#define FORMAT_TASK_STACK_SIZE 0xC00

typedef struct {
    bool transmit_mode_change_flag;           /* 记录当前是否在transmit change流程中 */
    ux_api_sys_transmit_mode_t transmit_mode; /* 记录当前生效的transmit mode */
} ux_sys_context_t;

static ux_api_file_format_handler format_finish_handler = NULL;
static ux_sys_context_t g_system_context = {0};

static ux_sys_context_t *ux_sys_get_context(void)
{
    return &g_system_context;
}

/* 上下文管理 */
ux_api_sys_transmit_mode_t ux_system_cfg_get_transmit_mode(void)
{
    ux_sys_context_t *ctx = ux_sys_get_context();
    return ctx->transmit_mode;
}

void ux_system_cfg_set_transmit_mode(ux_api_sys_transmit_mode_t mode)
{
    ux_sys_context_t *ctx = ux_sys_get_context();
    ctx->transmit_mode = mode;
}

bool ux_sys_cfg_get_transmit_mode_change_flag(void)
{
    ux_sys_context_t *ctx = ux_sys_get_context();
    return ctx->transmit_mode_change_flag;
}

void ux_sys_cfg_set_transmit_mode_change_flag(bool flag)
{
    ux_sys_context_t *ctx = ux_sys_get_context();
    ctx->transmit_mode_change_flag = flag;
}

static void ux_system_execute_format(void)
{
#ifdef CONFIG_SUPPORT_FILESYSTEM
    int format_ret = emmc_drv_format();
    if (format_ret != ENOERR) {
        ux_log_err("[ux][sys][ux_system_execute_format] Wrong with errcode: %d during format!");
    }

    if (format_finish_handler != NULL) {
        format_finish_handler(format_ret);
    }
    statfs_info_init();
#endif
}

errcode_t ux_api_sys_file_async_format(ux_api_file_format_handler handler)
{
    errcode_t ret = ERRCODE_SUCC;
    format_finish_handler = handler;
    if (ux_usb_get_device_type() == UX_USB_MASS_STORAGE) {
        ux_log_err("[ux][sys]Format not allowed in USB mode");
        return ERRCODE_FAIL;
    }
#ifdef CONFIG_SUPPORT_AUDIO_RECORDER
    if (ux_audio_get_local_recorder_status() == UX_AUDIO_RECORDER_STATUS_START) {
        ux_log_err("[ux][sys]Format not allowed in recording mode");
        return ERRCODE_FAIL;
    }
#endif
    osal_task *format_task_handle = osal_kthread_create((osal_kthread_handler)(ux_system_execute_format), NULL,
                                                        "FormatTask", FORMAT_TASK_STACK_SIZE);
    if (format_task_handle == NULL) {
        ret = ERRCODE_FAIL;
    }
    return ret;
}

#ifdef XTS_SUPPORT
bool ux_api_sys_get_xts_flag(void)
{
    return property_get_bool(NV_ID_SYS_XTS_FLAG, false);
}

errcode_t ux_api_sys_set_xts_flag(bool flag)
{
    errcode_t ret = property_set_bool(NV_ID_SYS_XTS_FLAG, flag);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uapi_power_reset();
    return ERRCODE_SUCC;
}
#endif

#ifdef CONFIG_SUPPORT_SYSTEM_TRANSMIT_MODE
/* 从nv中读取transmit mode */
static ux_api_sys_transmit_mode_t ux_sys_read_transmit_mode_from_nv(void)
{
    ux_api_sys_transmit_mode_t transmit_mode = UX_API_SYS_TRANSMIT_MODE_MAX;
    uint8_t temp_nv;
    uint8_t *p_ret = property_get_bytes(NV_ID_SYS_TRANSMIT_MODE, (uint8_t *)(&temp_nv), sizeof(uint8_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][sys]get nv transmit_mode failed");
        return UX_API_SYS_TRANSMIT_MODE_MAX;
    }
    transmit_mode = (ux_api_sys_transmit_mode_t)temp_nv;
    return transmit_mode;
}

/* 向nv中写入transmit mode */
static errcode_t ux_sys_write_transmit_mode_to_nv(ux_api_sys_transmit_mode_t transmit_mode)
{
    errcode_t ret = UX_ERR_SUCCESS;
    uint8_t temp_nv = (uint8_t)transmit_mode;
    ret = property_set_bytes(NV_ID_SYS_TRANSMIT_MODE, (uint8_t *)(&temp_nv), sizeof(uint8_t));
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][sys]property_set_bytes failed with errorcode 0x%X\r\n", ret);
        return ret;
    }
    return ret;
}

/* 内部接口，根据transmit mode设置音频帧长 */
static errcode_t ux_sys_update_audio_frame_length(ux_api_sys_transmit_mode_t transmit_mode)
{
    errcode_t ret;
    switch (transmit_mode) {
        case UX_API_SYS_TRANSMIT_MODE_1R2T:
            ux_audio_set_frame_length(UX_AUDIO_FRAME_LENGTH_5_MS);
            ret = ux_audio_recorder_set_frame_length(UX_AUDIO_FRAME_LENGTH_5_MS);
            break;
        case UX_API_SYS_TRANSMIT_MODE_1R4T:
        case UX_API_SYS_TRANSMIT_MODE_1R8T:
            ux_audio_set_frame_length(UX_AUDIO_FRAME_LENGTH_10_MS);
            ret = ux_audio_recorder_set_frame_length(UX_AUDIO_FRAME_LENGTH_10_MS);
            break;
        default:
            ux_audio_set_frame_length(UX_AUDIO_FRAME_LENGTH_10_MS);
            ret = ux_audio_recorder_set_frame_length(UX_AUDIO_FRAME_LENGTH_10_MS);
            break;
    }
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][sys]ux_audio_recorder_set_frame_length failed.\n");
        return ret;
    }
    return ret;
}
#endif

/* 内部接口，设置transmit mode */
errcode_t ux_sys_livemic_set_transmit_mode(ux_api_sys_transmit_mode_t transmit_mode)
{
    unused(transmit_mode);
    errcode_t ret = UX_ERR_SUCCESS;

#ifdef CONFIG_SUPPORT_SYSTEM_TRANSMIT_MODE
    ux_api_comm_mail_t mail = {0};
    /* 发消息给sle activity处理模式切换 */
    mail.event = UX_EVENT_TYPE_BG;
    mail.sub_event = UX_BG_SLE_TRANSMIT_MODE_CHANGE;
    ret = ux_api_fwk_push_mail(&mail, (uint8_t *)&transmit_mode, sizeof(uint8_t));
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][sys]ux_api_fwk_push_mail failed.\n");
        return ret;
    }
    /* 写入nv和上下文 */
    ret = ux_sys_write_transmit_mode_to_nv(transmit_mode);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][sys]ux_sys_write_transmit_mode_to_nv failed\r\n");
        return ret;
    }
    ux_system_cfg_set_transmit_mode(transmit_mode);
    /* 根据transmit mode配置音频帧长 */
    ret = ux_sys_update_audio_frame_length(transmit_mode);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][sys]ux_sys_update_audio_frame_length failed.\n");
        return ret;
    }
#endif
    return ret;
}

#ifdef MELODY_DEMO_BMIC_RX
static errcode_t ux_sys_rx_transmit_mode_change_process(ux_api_sys_transmit_mode_t transmit_mode)
{
    errcode_t ret;
    /* 关闭uac */
    ux_api_comm_mail_t mail = {0};
    mail.event = UX_EVENT_TYPE_USB;
    mail.sub_event = UX_USB_UAC_UPLINK_STOP;
    ret = ux_api_fwk_push_mail(&mail, NULL, 0);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][sys]ux_api_fwk_push_mail failed.\n");
        return ret;
    }
    /* 向所有已连接tx发送切换请求 */
    ret = ux_bs_rpc_send_transmit_mode_change_request(transmit_mode);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][sys] ux_bs_rpc_send_transmit_mode_change_request failed\r\n");
    }
    return UX_ERR_SUCCESS;
}
#endif

/* 对外接口，设置transmit mode，支持动态切换 */
errcode_t ux_api_sys_set_transmit_mode(ux_api_sys_transmit_mode_t transmit_mode)
{
    unused(transmit_mode);
    errcode_t ret = UX_ERR_SUCCESS;

#ifdef CONFIG_SUPPORT_SYSTEM_TRANSMIT_MODE
    /* 参数校验 */
    if (transmit_mode != UX_API_SYS_TRANSMIT_MODE_1R2T &&
        transmit_mode != UX_API_SYS_TRANSMIT_MODE_1R4T &&
        transmit_mode != UX_API_SYS_TRANSMIT_MODE_1R8T) {
        ux_log_err("[ux][sys]Invalid transmit mode.\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    /* 切换准入 */
    bool flag = ux_sys_cfg_get_transmit_mode_change_flag();
    if (flag == true) {
        ux_log_info("[ux][sys] Transmit mode switching\r\n");
        return UX_ERR_SUCCESS;
    }
    ux_api_sys_transmit_mode_t current_mode = ux_system_cfg_get_transmit_mode();
    if (transmit_mode == current_mode) {
        ux_log_info("[ux][sys] Transmit mode doesn't need to switch, current mode: %d\r\n", current_mode);
        return UX_ERR_SUCCESS;
    }
    ux_sys_cfg_set_transmit_mode_change_flag(true);
#ifdef MELODY_DEMO_BMIC_RX
    ret = ux_sys_rx_transmit_mode_change_process(transmit_mode);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][sys] ux_sys_rx_transmit_mode_change_process failed\r\n");
        ux_sys_cfg_set_transmit_mode_change_flag(false);
        return ret;
    }
#endif
    /* 自身切换 */
    ret = ux_sys_livemic_set_transmit_mode(transmit_mode);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][sys] ux_sys_livemic_set_transmit_mode failed\r\n");
        ux_sys_cfg_set_transmit_mode_change_flag(false);
        return ret;
    }
#endif
    return ret;
}

/* 对外接口，查询transmit mode */
ux_api_sys_transmit_mode_t ux_api_sys_get_transmit_mode(void)
{
    return ux_system_cfg_get_transmit_mode();
}

/* 内部接口，用于每次上电时读取transmit mode至上下文 */
errcode_t ux_system_cfg_restore_transmit_mode(void)
{
    errcode_t ret = UX_ERR_SUCCESS;
#ifdef CONFIG_SUPPORT_SYSTEM_TRANSMIT_MODE
    /* 读取nv写入上下文 */
    ux_sys_context_t *ctx = ux_sys_get_context();
    ux_api_sys_transmit_mode_t transmit_mode = ux_sys_read_transmit_mode_from_nv();
    if (transmit_mode == UX_API_SYS_TRANSMIT_MODE_MAX) {
        ux_log_err("[ux][sys]Invalid parameter\r\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    ctx->transmit_mode = transmit_mode;
    /* 上下文初始化，初始化为false代表当前没有在切换流程中 */
    ux_sys_cfg_set_transmit_mode_change_flag(false);
    /* 音频帧长默认配置 */
    ret = ux_sys_update_audio_frame_length(transmit_mode);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][sys]ux_sys_update_audio_frame_length failed\r\n");
        return ret;
    }
#endif
    return ret;
}

/* 内部接口，用于查询最大支持的可同时连接的对端设备数量 */
uint8_t ux_sys_get_max_connect_num(void)
{
#ifdef MELODY_DEMO_BMIC_RX
    /* 直播麦RX：根据transmit mode判断 */
    ux_api_sys_transmit_mode_t mode = ux_system_cfg_get_transmit_mode();
    switch (mode) {
        case UX_API_SYS_TRANSMIT_MODE_1R2T:
            return (uint8_t)UX_SYS_MAX_CONNECT_NUM_2;
        case UX_API_SYS_TRANSMIT_MODE_1R4T:
            return (uint8_t)UX_SYS_MAX_CONNECT_NUM_4;
        case UX_API_SYS_TRANSMIT_MODE_1R8T:
            return (uint8_t)UX_SYS_MAX_CONNECT_NUM_8;
        default:
            return (uint8_t)UX_SYS_MAX_CONNECT_NUM_MAX;
    }
#endif
#ifdef MELODY_DEMO_BMIC_TX
    /* 直播麦TX：最大只支持同时连接1个RX */
    return (uint8_t)UX_SYS_MAX_CONNECT_NUM_1;
#endif
    return (uint8_t)UX_SYS_MAX_CONNECT_NUM_MAX;
}

errcode_t ux_api_sys_set_time(calendar_t *calendar)
{
    if (calendar == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    return ux_sys_set_time(*calendar);
}

errcode_t ux_api_sys_get_sdk_version_by_id(uint16_t device_id, char* sdk_version, uint16_t data_len)
{
    return ux_system_cfg_get_sdk_version(device_id, sdk_version, data_len);
}

errcode_t ux_api_sys_set_language(uint8_t language)
{
    return ux_config_set_language(language);
}

uint8_t ux_api_sys_get_language(void)
{
    return ux_config_get_language();
}
#endif