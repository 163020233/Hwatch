/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag oam log
 * This file should be changed only infrequently and with great care.
 */
#include "securec.h"
#include "diag.h"
#include "soc_diag_cmd_id.h"
#include "soc_diag_util.h"
#include "diag_common.h"
#include "diag_filter.h"
#include "diag_adapt_layer.h"
#include "log_types.h"
#include "errcode.h"
#include "diag_adapt_sdt.h"
#ifdef SUPPORT_CONNECTIVITY
#include "connectivity_log.h"
#endif
#include "ipc_porting.h"

#define CORE_ID_OFFSET 13

oam_cmd_handle_callback g_diag_debug_proc = NULL;

static bool diag_adapt_hso_msg_report(uint8_t *data, uint32_t data_len)
{
    diag_router_frame_t *frame = NULL;
    diag_router_inner_frame_t *inner = NULL;

    if (data_len >= sizeof(diag_router_frame_t)) {
        frame = (diag_router_frame_t *)data;
        inner = (diag_router_inner_frame_t *)frame->inner;
        uint16_t frame_len = DIAG_ROUTER_HEADER_LEN;
        if (get_frame_ctrl_extend_en(frame->ctrl) != 0) {
            inner = (diag_router_inner_frame_t *)(frame->inner + DIAG_ROUTER_CTRL2_LEN);
            frame_len += DIAG_ROUTER_CTRL2_LEN;
        }
        frame_len += get_frame_len(inner->len_msb, inner->len_lsb);
        if ((frame->sof == DIAG_ROUTER_FRAME_START_FLAG) &&
            (frame_len == data_len) &&
            ((get_frame_ctrl_eof_en(frame->ctrl) == 0) ||
            (data[data_len - 1] == DIAG_ROUTER_FRAME_END_FLAG))) {
            uapi_diag_report_sys_msg_from_buffer(data, data_len, false);
            return true;
        }
    }
    return false;
}

void zdiag_adapt_sdt_msg_proc(uint8_t *buf1, uint32_t len1, uint8_t *buf2, uint32_t len2)
{
    uint32_t len = len1 + len2;
    uint8_t *buf = NULL;
    uint32_t pos = 0;

    if (len > 0) {
        buf = dfx_malloc(0, len);
    }

    if (buf == NULL) {
        return;
    }

    if (len1 > 0) {
        (void)memcpy_s(buf + pos, len1, buf1, len1);
        pos += len1;
    }

    if (len2 > 0) {
        (void)memcpy_s(buf + pos, len2, buf2, len2);
    }

    /* 如果是diag日志帧，直接从串口发出 */
    diag_adapt_hso_msg_report(buf, len);
    dfx_free(0, buf);
    return;
}

void diag_register_debug_cmd_callback(oam_cmd_handle_callback func)
{
    g_diag_debug_proc =  func;
}

void diag_debug_cmd_proc(uint8_t *data, uint32_t len)
{
    if (g_diag_debug_proc != NULL) {
        g_diag_debug_proc(data, (uint16_t)len);
    }
}

#ifndef FORBID_AUTO_LOG_REPORT
void diag_auto_log_report_enable(void)
{
    uint32_t level[] = {
        DIAG_LOG_LEVEL_DBG,
        DIAG_LOG_LEVEL_INFO,
        DIAG_LOG_LEVEL_WARN,
        DIAG_LOG_LEVEL_ERROR,
    };
    uint32_t mod_id[] = {
        LOG_WIFIMODULE,
        LOG_BTMODULE | (BT << CORE_ID_OFFSET),
        LOG_DSPMODULE | (PROTOCOL << CORE_ID_OFFSET),
        LOG_PFMODULE | (BT << CORE_ID_OFFSET),
        LOG_PFMODULE | (APPS << CORE_ID_OFFSET),
        LOG_PFMODULE | (GNSS << CORE_ID_OFFSET),
        LOG_GNSSMODULE | (GNSS << CORE_ID_OFFSET),
        LOG_MEDIAMODULE | (APPS << CORE_ID_OFFSET),
        LOG_APPMODULE | (APPS << CORE_ID_OFFSET),
        LOG_BTSTATUS_MODULE | (BT << CORE_ID_OFFSET),
        LOG_BTOTA_MODULE | (BT << CORE_ID_OFFSET),
        LOG_BTHMODULE | (BT << CORE_ID_OFFSET),
        LOG_BGHMODULE | (APPS << CORE_ID_OFFSET),
        LOG_BTMODULE | (APPS << CORE_ID_OFFSET),
    };

#ifndef FT_SINGLE_UART
    /* 全局使能 */
    zdiag_set_enable(true, (uint8_t)diag_adapt_get_default_dst());
#endif
    /* module 使能 */
    for (uint32_t i = 0; i < sizeof(mod_id) / sizeof(uint32_t); i++) {
        zdiag_set_id_enable(mod_id[i], true);
    }

    /* level 使能 */
    for (uint32_t i = 0; i < sizeof(level) / sizeof(uint32_t); i++) {
        zdiag_set_level_enable((uint8_t)level[i], true);
    }
}
#endif

#if (CORE_NUMS > 1)

void diag_highest_level_proc(uint8_t level)
{
    log_level_e core_level = LOG_LEVEL_NONE;
    log_level_e temp;
    cores_t cores[] = { CORES_APPS_CORE, CORES_BT_CORE};

    if (level == DIAG_LOG_LEVEL_DBG) {
        core_level = LOG_LEVEL_DBG;
    } else if (level == DIAG_LOG_LEVEL_INFO) {
        core_level = LOG_LEVEL_INFO;
    } else if (level == DIAG_LOG_LEVEL_WARN) {
        core_level = LOG_LEVEL_WARNING;
    } else if (level == DIAG_LOG_LEVEL_ERROR) {
        core_level = LOG_LEVEL_ERROR;
    }
#if !defined(BUILD_RECOVERY_IMAGE) && !defined(TEST_SUIT)
    uapi_audio_set_dsp_log_level(core_level); // 添加dsp level 管控
#endif
    for (uint32_t i = 0; i < sizeof(cores) / sizeof(cores_t); i++) {
        get_log_level(cores[i], &temp);
        if (cores[i] != CORES_APPS_CORE) {
#ifdef SUPPORT_IPC
            if (ipc_check_status(cores[i]) != IPC_STATUS_OK) {
                dfx_log_err("ipc_check_status error. core = %d\r\n", cores[i]);
                continue;
            }
#endif
        }
        set_log_level(cores[i], core_level);
        dfx_log_info("core log level %d, %d, %d OK\r\n", cores[i], temp, core_level);
    }
}
#endif

void diag_id_proc(uint32_t id, bool enable)
{
    if (id == (LOG_DSPMODULE | (PROTOCOL << CORE_ID_OFFSET))) {
    #if !defined(BUILD_RECOVERY_IMAGE) && !defined(TEST_SUIT)
        log_level_e core_level = LOG_LEVEL_INFO;
        core_level = enable ? LOG_LEVEL_INFO : LOG_LEVEL_NONE;
        uapi_audio_set_dsp_log_level(core_level); // 添加dsp level 管控
    #else
        unused(enable);
    #endif
    }
}

void zdiag_level_proc(uint8_t level_bits)
{
    uint8_t level = DIAG_LOG_LEVEL_ALERT;
    if (level_bits != 0) {
        for (int8_t i = DIAG_LOG_LEVEL_TRACE; i >= 0; i--) {
            if (level_bits & (1U << i)) {
                level = i;
                break;
            }
        }
    }
    diag_highest_level_proc(level);
}