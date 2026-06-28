/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description:   LOG OAM LOGGER MODULE
 */
#include "log_oam_logger.h"

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
#include <stdarg.h>
#include "log_common.h"

#if defined(CHIP_CAT1) && (CHIP_CAT1 == 1) && (CORE != CORE_LOGGING)
#else
#include "uart.h"
#endif

#ifdef CONFIG_LOG_USE_DIAG_FRAME
#include "diag_ind_src.h"
#endif

#ifdef BTH_LOG_BUG_FIX
#define LOG_LEVEL_DEBUG_TO_ERROR 0x40
#endif

#ifdef CFG_DRIVERS_USB_AS_SERIAL
#include "usb_porting.h"
#endif

/* Module log level defination */
om_log_module_lev_t g_module_log_level[MODULEID_BUTT] = {
    { LOG_WIFIMODULE,   LOG_LEVEL_INFO },
    { LOG_BTMODULE,     LOG_LEVEL_INFO },
    { LOG_BTHMODULE,   LOG_LEVEL_INFO },
    { LOG_NFCMODULE,   LOG_LEVEL_INFO },
    { LOG_PFMODULE,     LOG_LEVEL_INFO },
};

#ifdef CONFIG_DFX_SUPPORT_USERS_PRINT
static log_other_print_t g_other_print_func = NULL;

void log_other_print_register(log_other_print_t users_print_func)
{
    if (users_print_func == NULL) {
        return;
    }
    g_other_print_func = users_print_func;
}

void log_other_print0(uint32_t log_header, uint32_t log_level, const char *fmt)
{
    if (g_other_print_func == NULL) {
        return;
    }
    g_other_print_func(log_header, log_level, fmt, 0);
}

void log_other_print1(uint32_t log_header, uint32_t log_level, const char *fmt, uint32_t p0)
{
    if (g_other_print_func == NULL) {
        return;
    }
    g_other_print_func(log_header, log_level, fmt, 1, p0);
}

void log_other_print2(uint32_t log_header, uint32_t log_level, const char *fmt, uint32_t p0, uint32_t p1)
{
    if (g_other_print_func == NULL) {
        return;
    }
    g_other_print_func(log_header, log_level, fmt, LOG_OAM_INDEX_2, p0, p1);
}

void log_other_print3(uint32_t log_header, uint32_t log_level, const char *fmt, uint32_t p0, uint32_t p1, uint32_t p2)
{
    if (g_other_print_func == NULL) {
        return;
    }
    g_other_print_func(log_header, log_level, fmt, LOG_OAM_INDEX_3, p0, p1, p2);
}

void log_other_print4(uint32_t log_header, uint32_t log_level, const char *fmt,
                      uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3)
{
    if (g_other_print_func == NULL) {
        return;
    }
    g_other_print_func(log_header, log_level, fmt, LOG_OAM_INDEX_4, p0, p1, p2, p3);
}
#endif

static bool log_check(uint32_t *presspara)
{
    if (get_module_id(*presspara) >= MODULEID_BUTT) {
        return false;
    }

#ifdef BTH_LOG_BUG_FIX
    if (get_module_id(*presspara) == LOG_BTHMODULE && getlog_level(*presspara) == LOG_LEVEL_NONE) {
        *presspara |= LOG_LEVEL_DEBUG_TO_ERROR;
    }
#endif

    if (getlog_level(*presspara) > (uint8_t)log_get_local_log_level()) {
        return false;
    }
    return true;
}

void log_event_print0(uint32_t log_header, uint32_t presspara)
{
    // Module ID is unknown
    uint32_t presspara_temp = presspara;
    if (log_check(&presspara_temp) == false) { return; }

#ifndef CONFIG_LOG_USE_DIAG_FRAME
    uint32_t log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + OML_LOG_ZERO_ARG_SEND + OML_LOG_TAIL_LENTH];

    log_oam_entry[LOG_OAM_INDEX_0] = log_header;
    log_oam_entry[LOG_OAM_INDEX_1] =
        (uint32_t)log_lenth_and_sn_press(OML_LOG_ZERO_ARG_SEND, (uint32_t)get_log_sn_number());
    log_oam_entry[LOG_OAM_INDEX_2] = presspara_temp;
    log_oam_entry[LOG_OAM_INDEX_3] = OM_FRAME_DELIMITER;

    log_event((uint8_t *)log_oam_entry, oal_log_lenth(OML_LOG_ZERO_ARG_SEND));
#else
    uint32_t module_id = log_header;
    uint8_t level = getlog_level(presspara_temp);
    level = sdt_to_diag_log_level(level);
    uint32_t msg_id = (presspara_temp & ~OAM_LOG_LEVEL_MASK) | level;

    uapi_diag_report_sys_msg_to_buffer(module_id, msg_id, NULL, 0, level);
#endif
}

void log_event_print1(uint32_t log_header, uint32_t presspara, uint32_t para1)
{
    // Module ID is unknown
    uint32_t presspara_temp = presspara;
    if (log_check(&presspara_temp) == false) { return; }

#ifndef CONFIG_LOG_USE_DIAG_FRAME
    uint32_t log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + OML_LOG_ONE_ARG_SEND + OML_LOG_TAIL_LENTH];

    log_oam_entry[LOG_OAM_INDEX_0] = log_header;
    log_oam_entry[LOG_OAM_INDEX_1] =
        (uint32_t)log_lenth_and_sn_press(OML_LOG_ONE_ARG_SEND, (uint32_t)get_log_sn_number());
    log_oam_entry[LOG_OAM_INDEX_2] = presspara_temp;
    log_oam_entry[LOG_OAM_INDEX_3] = para1;
    log_oam_entry[LOG_OAM_INDEX_4] = OM_FRAME_DELIMITER;

    log_event((uint8_t *)log_oam_entry, oal_log_lenth(OML_LOG_ONE_ARG_SEND));
#else
    uint32_t module_id = log_header;
    uint8_t level = getlog_level(presspara_temp);
    level = sdt_to_diag_log_level(level);
    uint32_t msg_id = (presspara_temp & ~OAM_LOG_LEVEL_MASK) | level;
    uint32_t log_data[OML_LOG_ONE_ARG_SEND];

    log_data[LOG_OAM_INDEX_0] = para1;

    uapi_diag_report_sys_msg_to_buffer(module_id, msg_id, (uint8_t *)log_data, sizeof(log_data), level);
#endif
}

void log_event_print2(uint32_t log_header, uint32_t presspara, uint32_t para1, uint32_t para2)
{
    // Module ID is unknown
    uint32_t presspara_temp = presspara;
    if (log_check(&presspara_temp) == false) { return; }

#ifndef CONFIG_LOG_USE_DIAG_FRAME
    uint32_t log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + OML_LOG_TWO_ARG_SEND + OML_LOG_TAIL_LENTH];

    log_oam_entry[LOG_OAM_INDEX_0] = log_header;
    log_oam_entry[LOG_OAM_INDEX_1] =
        (uint32_t)log_lenth_and_sn_press(OML_LOG_TWO_ARG_SEND, (uint32_t)get_log_sn_number());
    log_oam_entry[LOG_OAM_INDEX_2] = presspara_temp;
    log_oam_entry[LOG_OAM_INDEX_3] = para1;
    log_oam_entry[LOG_OAM_INDEX_4] = para2;
    log_oam_entry[LOG_OAM_INDEX_5] = OM_FRAME_DELIMITER;

    log_event((uint8_t *)log_oam_entry, oal_log_lenth(OML_LOG_TWO_ARG_SEND));
#else
    uint32_t module_id = log_header;
    uint8_t level = getlog_level(presspara_temp);
    level = sdt_to_diag_log_level(level);
    uint32_t msg_id = (presspara_temp & ~OAM_LOG_LEVEL_MASK) | level;
    uint32_t log_data[OML_LOG_TWO_ARG_SEND];

    log_data[LOG_OAM_INDEX_0] = para1;
    log_data[LOG_OAM_INDEX_1] = para2;

    uapi_diag_report_sys_msg_to_buffer(module_id, msg_id, (uint8_t *)log_data, sizeof(log_data), level);
#endif
}

void log_event_print3(uint32_t log_header, uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    // Module ID is unknown
    uint32_t presspara_temp = presspara;
    if (log_check(&presspara_temp) == false) { return; }

#ifndef CONFIG_LOG_USE_DIAG_FRAME
    uint32_t log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + OML_LOG_THREE_ARG_SEND + OML_LOG_TAIL_LENTH];

    log_oam_entry[LOG_OAM_INDEX_0] = log_header;
    log_oam_entry[LOG_OAM_INDEX_1] =
        (uint32_t)log_lenth_and_sn_press(OML_LOG_THREE_ARG_SEND, (uint32_t)get_log_sn_number());
    log_oam_entry[LOG_OAM_INDEX_2] = presspara_temp;
    log_oam_entry[LOG_OAM_INDEX_3] = para1;
    log_oam_entry[LOG_OAM_INDEX_4] = para2;
    log_oam_entry[LOG_OAM_INDEX_5] = para3;
    log_oam_entry[LOG_OAM_INDEX_6] = OM_FRAME_DELIMITER;

    log_event((uint8_t *)log_oam_entry, oal_log_lenth(OML_LOG_THREE_ARG_SEND));
#else
    uint32_t module_id = log_header;
    uint8_t level = getlog_level(presspara_temp);
    level = sdt_to_diag_log_level(level);
    uint32_t msg_id = (presspara_temp & ~OAM_LOG_LEVEL_MASK) | level;
    uint32_t log_data[OML_LOG_THREE_ARG_SEND];

    log_data[LOG_OAM_INDEX_0] = para1;
    log_data[LOG_OAM_INDEX_1] = para2;
    log_data[LOG_OAM_INDEX_2] = para3;

    uapi_diag_report_sys_msg_to_buffer(module_id, msg_id, (uint8_t *)log_data, sizeof(log_data), level);
#endif
}

void log_event_print4(uint32_t log_header, uint32_t presspara, uint32_t para1, uint32_t para2,
                      uint32_t para3, uint32_t para4)
{
    // Module ID is unknown
    uint32_t presspara_temp = presspara;
    if (log_check(&presspara_temp) == false) { return; }

#ifndef CONFIG_LOG_USE_DIAG_FRAME
    uint32_t log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + OML_LOG_FOUR_ARG_SEND + OML_LOG_TAIL_LENTH];

    log_oam_entry[LOG_OAM_INDEX_0] = log_header;
    log_oam_entry[LOG_OAM_INDEX_1] =
        (uint32_t)log_lenth_and_sn_press(OML_LOG_FOUR_ARG_SEND, (uint32_t)get_log_sn_number());
    log_oam_entry[LOG_OAM_INDEX_2] = presspara_temp;
    log_oam_entry[LOG_OAM_INDEX_3] = para1;
    log_oam_entry[LOG_OAM_INDEX_4] = para2;
    log_oam_entry[LOG_OAM_INDEX_5] = para3;
    log_oam_entry[LOG_OAM_INDEX_6] = para4;
    log_oam_entry[LOG_OAM_INDEX_7] = OM_FRAME_DELIMITER;

    log_event((uint8_t *)log_oam_entry, oal_log_lenth(OML_LOG_FOUR_ARG_SEND));
#else
    uint32_t module_id = log_header;
    uint8_t level = getlog_level(presspara_temp);
    level = sdt_to_diag_log_level(level);
    uint32_t msg_id = (presspara_temp & ~OAM_LOG_LEVEL_MASK) | level;
    uint32_t log_data[OML_LOG_FOUR_ARG_SEND];

    log_data[LOG_OAM_INDEX_0] = para1;
    log_data[LOG_OAM_INDEX_1] = para2;
    log_data[LOG_OAM_INDEX_2] = para3;
    log_data[LOG_OAM_INDEX_3] = para4;
    uapi_diag_report_sys_msg_to_buffer(module_id, msg_id, (uint8_t *)log_data, sizeof(log_data), level);
#endif
}

void log_event_print_alterable_para_press(uint32_t log_header, uint32_t presspara, uint32_t paranum, ...)
{
    uint8_t uc_para_num;
    uint8_t uc_loop;
    va_list args;

    // Module ID is unknown
    uint32_t presspara_temp = presspara;
    if (log_check(&presspara_temp) == false) { return; }

    uc_para_num = (uint8_t)((paranum > OML_LOG_ALTER_PARA_MAX_NUM) ? (OML_LOG_ALTER_PARA_MAX_NUM) : paranum);

#ifndef CONFIG_LOG_USE_DIAG_FRAME
    uint32_t log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + OML_LOG_ALTER_PARA_MAX_NUM + OML_LOG_TAIL_LENTH];

    log_oam_entry[LOG_OAM_INDEX_0] = log_header;
    log_oam_entry[LOG_OAM_INDEX_1] = (uint32_t)log_lenth_and_sn_press(uc_para_num, (uint32_t)get_log_sn_number());
    log_oam_entry[LOG_OAM_INDEX_2] = presspara_temp;

    va_start(args, paranum);
    for (uc_loop = 0; uc_loop < uc_para_num; uc_loop++) {
        log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + uc_loop] = (uint32_t)va_arg(args, uint32_t);
    }
    va_end(args);
    log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + uc_loop] = OM_FRAME_DELIMITER;

    log_event((uint8_t *)log_oam_entry, oal_log_lenth(uc_para_num));
#else
    uint32_t module_id = log_header;
    uint8_t level = getlog_level(presspara_temp);
    level = sdt_to_diag_log_level(level);
    uint32_t msg_id = (presspara_temp & ~OAM_LOG_LEVEL_MASK) | level;
    uint32_t log_data[OML_LOG_ALTER_PARA_MAX_NUM];

    va_start(args, paranum);
    for (uc_loop = 0; uc_loop < uc_para_num; uc_loop++) {
        log_data[uc_loop] = (uint32_t)va_arg(args, uint32_t);
    }
    va_end(args);

    uapi_diag_report_sys_msg_to_buffer(module_id, msg_id, (uint8_t *)log_data, sizeof(uint32_t) * uc_para_num, level);
#endif
}

/* 以下为优化日志接口，去掉一个参数 */
/* BTC */
__attribute__((weak)) void log_event_bt_print0(uint32_t presspara)
{
    log_event_print0(log_head_press(OM_BT), presspara);
}

__attribute__((weak)) void log_event_bt_print1(uint32_t presspara, uint32_t para1)
{
    log_event_print1(log_head_press(OM_BT), presspara, para1);
}

__attribute__((weak)) void log_event_bt_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    log_event_print2(log_head_press(OM_BT), presspara, para1, para2);
}

__attribute__((weak)) void log_event_bt_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    log_event_print3(log_head_press(OM_BT), presspara, para1, para2, para3);
}

__attribute__((weak)) void log_event_bt_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3,
                                               uint32_t para4)
{
    log_event_print4(log_head_press(OM_BT), presspara, para1, para2, para3, para4);
}

/* BTH */
__attribute__((weak)) void log_event_bth_print0(uint32_t presspara)
{
    log_event_print0(log_head_press(OM_BTH), presspara);
}

__attribute__((weak)) void log_event_bth_print1(uint32_t presspara, uint32_t para1)
{
    log_event_print1(log_head_press(OM_BTH), presspara, para1);
}

__attribute__((weak)) void log_event_bth_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    log_event_print2(log_head_press(OM_BTH), presspara, para1, para2);
}

__attribute__((weak)) void log_event_bth_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    log_event_print3(log_head_press(OM_BTH), presspara, para1, para2, para3);
}

__attribute__((weak)) void log_event_bth_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3,
                                                uint32_t para4)
{
    log_event_print4(log_head_press(OM_BTH), presspara, para1, para2, para3, para4);
}

/* PLT */
__attribute__((weak)) void log_event_pf_print0(uint32_t presspara)
{
    log_event_print0(log_head_press(OM_PF), presspara);
}

__attribute__((weak)) void log_event_pf_print1(uint32_t presspara, uint32_t para1)
{
    log_event_print1(log_head_press(OM_PF), presspara, para1);
}

__attribute__((weak)) void log_event_pf_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    log_event_print2(log_head_press(OM_PF), presspara, para1, para2);
}

__attribute__((weak)) void log_event_pf_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    log_event_print3(log_head_press(OM_PF), presspara, para1, para2, para3);
}

__attribute__((weak)) void log_event_pf_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3,
                                               uint32_t para4)
{
    log_event_print4(log_head_press(OM_PF), presspara, para1, para2, para3, para4);
}

/* ALG */
__attribute__((weak)) void log_event_alg_print0(uint32_t presspara)
{
    log_event_print0(log_head_press(OM_ALG), presspara);
}

__attribute__((weak)) void log_event_alg_print1(uint32_t presspara, uint32_t para1)
{
    log_event_print1(log_head_press(OM_ALG), presspara, para1);
}

__attribute__((weak)) void log_event_alg_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    log_event_print2(log_head_press(OM_ALG), presspara, para1, para2);
}

__attribute__((weak)) void log_event_alg_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    log_event_print3(log_head_press(OM_ALG), presspara, para1, para2, para3);
}

__attribute__((weak)) void log_event_alg_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3,
                                                uint32_t para4)
{
    log_event_print4(log_head_press(OM_ALG), presspara, para1, para2, para3, para4);
}

/* WIFI */
void log_event_wifi_print0(uint32_t presspara)
{
    log_event_print0(log_head_press(OM_WIFI), presspara);
}

void log_event_wifi_print1(uint32_t presspara, uint32_t para1)
{
    log_event_print1(log_head_press(OM_WIFI), presspara, para1);
}

void log_event_wifi_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    log_event_print2(log_head_press(OM_WIFI), presspara, para1, para2);
}

void log_event_wifi_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    log_event_print3(log_head_press(OM_WIFI), presspara, para1, para2, para3);
}

void log_event_wifi_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4)
{
    log_event_print4(log_head_press(OM_WIFI), presspara, para1, para2, para3, para4);
}

/* BGH */
void log_event_bgh_print0(uint32_t presspara)
{
    log_event_print0(log_head_press(OM_BGH), presspara);
}

void log_event_bgh_print1(uint32_t presspara, uint32_t para1)
{
    log_event_print1(log_head_press(OM_BGH), presspara, para1);
}

void log_event_bgh_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    log_event_print2(log_head_press(OM_BGH), presspara, para1, para2);
}

void log_event_bgh_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    log_event_print3(log_head_press(OM_BGH), presspara, para1, para2, para3);
}

void log_event_bgh_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4)
{
    log_event_print4(log_head_press(OM_BGH), presspara, para1, para2, para3, para4);
}

/* 以下为进一步优化日志接口， 按level分类 */
#if defined(CONFIG_LOG_USE_OPTIMIZE_V2)

/* BT模块按level分类的打印接口 */
__attribute__((weak)) void log_event_bt_err_print0(uint32_t presspara)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print0(log_head_press(OM_BT), presspara_temp);
}

__attribute__((weak)) void log_event_bt_err_print1(uint32_t presspara, uint32_t para1)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print1(log_head_press(OM_BT), presspara_temp, para1);
}

__attribute__((weak)) void log_event_bt_err_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print2(log_head_press(OM_BT), presspara_temp, para1, para2);
}

__attribute__((weak)) void log_event_bt_err_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print3(log_head_press(OM_BT), presspara_temp, para1, para2, para3);
}

__attribute__((weak)) void log_event_bt_err_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3,
                                                   uint32_t para4)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print4(log_head_press(OM_BT), presspara_temp, para1, para2, para3, para4);
}

__attribute__((weak)) void log_event_bt_wrn_print0(uint32_t presspara)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print0(log_head_press(OM_BT), presspara_temp);
}

__attribute__((weak)) void log_event_bt_wrn_print1(uint32_t presspara, uint32_t para1)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print1(log_head_press(OM_BT), presspara_temp, para1);
}

__attribute__((weak)) void log_event_bt_wrn_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print2(log_head_press(OM_BT), presspara_temp, para1, para2);
}

__attribute__((weak)) void log_event_bt_wrn_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print3(log_head_press(OM_BT), presspara_temp, para1, para2, para3);
}

__attribute__((weak)) void log_event_bt_wrn_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3,
                                                   uint32_t para4)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print4(log_head_press(OM_BT), presspara_temp, para1, para2, para3, para4);
}

__attribute__((weak)) void log_event_bt_info_print0(uint32_t presspara)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print0(log_head_press(OM_BT), presspara_temp);
}

__attribute__((weak)) void log_event_bt_info_print1(uint32_t presspara, uint32_t para1)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print1(log_head_press(OM_BT), presspara_temp, para1);
}

__attribute__((weak)) void log_event_bt_info_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print2(log_head_press(OM_BT), presspara_temp, para1, para2);
}

__attribute__((weak)) void log_event_bt_info_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print3(log_head_press(OM_BT), presspara_temp, para1, para2, para3);
}

__attribute__((weak)) void log_event_bt_info_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3,
                                                    uint32_t para4)
{
    uint32_t presspara_temp =
        para_press(LOG_BTMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print4(log_head_press(OM_BT), presspara_temp, para1, para2, para3, para4);
}

/* BTH模块按level分类的打印接口 */
__attribute__((weak)) void log_event_bth_err_print0(uint32_t presspara)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print0(log_head_press(OM_BTH), presspara_temp);
}

__attribute__((weak)) void log_event_bth_err_print1(uint32_t presspara, uint32_t para1)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print1(log_head_press(OM_BTH), presspara_temp, para1);
}

__attribute__((weak)) void log_event_bth_err_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print2(log_head_press(OM_BTH), presspara_temp, para1, para2);
}

__attribute__((weak)) void log_event_bth_err_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print3(log_head_press(OM_BTH), presspara_temp, para1, para2, para3);
}

__attribute__((weak)) void log_event_bth_err_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3,
                                                    uint32_t para4)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print4(log_head_press(OM_BTH), presspara_temp, para1, para2, para3, para4);
}

__attribute__((weak)) void log_event_bth_wrn_print0(uint32_t presspara)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print0(log_head_press(OM_BTH), presspara_temp);
}

__attribute__((weak)) void log_event_bth_wrn_print1(uint32_t presspara, uint32_t para1)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print1(log_head_press(OM_BTH), presspara_temp, para1);
}

__attribute__((weak)) void log_event_bth_wrn_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print2(log_head_press(OM_BTH), presspara_temp, para1, para2);
}

__attribute__((weak)) void log_event_bth_wrn_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print3(log_head_press(OM_BTH), presspara_temp, para1, para2, para3);
}

__attribute__((weak)) void log_event_bth_wrn_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3,
                                                    uint32_t para4)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print4(log_head_press(OM_BTH), presspara_temp, para1, para2, para3, para4);
}

__attribute__((weak)) void log_event_bth_info_print0(uint32_t presspara)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print0(log_head_press(OM_BTH), presspara_temp);
}

__attribute__((weak)) void log_event_bth_info_print1(uint32_t presspara, uint32_t para1)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print1(log_head_press(OM_BTH), presspara_temp, para1);
}

__attribute__((weak)) void log_event_bth_info_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print2(log_head_press(OM_BTH), presspara_temp, para1, para2);
}

__attribute__((weak)) void log_event_bth_info_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print3(log_head_press(OM_BTH), presspara_temp, para1, para2, para3);
}

__attribute__((weak)) void log_event_bth_info_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3,
                                                     uint32_t para4)
{
    uint32_t presspara_temp =
        para_press(LOG_BTHMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print4(log_head_press(OM_BTH), presspara_temp, para1, para2, para3, para4);
}

/* BGH模块按level分类的打印接口 */
void log_event_bgh_err_print0(uint32_t presspara)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print0(log_head_press(OM_BGH), presspara_temp);
}

void log_event_bgh_err_print1(uint32_t presspara, uint32_t para1)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print1(log_head_press(OM_BGH), presspara_temp, para1);
}

void log_event_bgh_err_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print2(log_head_press(OM_BGH), presspara_temp, para1, para2);
}

void log_event_bgh_err_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print3(log_head_press(OM_BGH), presspara_temp, para1, para2, para3);
}

void log_event_bgh_err_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_ERROR, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print4(log_head_press(OM_BGH), presspara_temp, para1, para2, para3, para4);
}

void log_event_bgh_wrn_print0(uint32_t presspara)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print0(log_head_press(OM_BGH), presspara_temp);
}

void log_event_bgh_wrn_print1(uint32_t presspara, uint32_t para1)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print1(log_head_press(OM_BGH), presspara_temp, para1);
}

void log_event_bgh_wrn_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print2(log_head_press(OM_BGH), presspara_temp, para1, para2);
}

void log_event_bgh_wrn_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print3(log_head_press(OM_BGH), presspara_temp, para1, para2, para3);
}

void log_event_bgh_wrn_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_WARNING, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print4(log_head_press(OM_BGH), presspara_temp, para1, para2, para3, para4);
}

void log_event_bgh_info_print0(uint32_t presspara)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print0(log_head_press(OM_BGH), presspara_temp);
}

void log_event_bgh_info_print1(uint32_t presspara, uint32_t para1)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print1(log_head_press(OM_BGH), presspara_temp, para1);
}

void log_event_bgh_info_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print2(log_head_press(OM_BGH), presspara_temp, para1, para2);
}

void log_event_bgh_info_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print3(log_head_press(OM_BGH), presspara_temp, para1, para2, para3);
}

void log_event_bgh_info_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4)
{
    uint32_t presspara_temp =
        para_press(LOG_BGHMODULE, LOG_LEVEL_INFO, get_fileid_opt(presspara), get_line_opt(presspara));
    log_event_print4(log_head_press(OM_BGH), presspara_temp, para1, para2, para3, para4);
}

#endif /* CONFIG_LOG_USE_OPTIMIZE_V2 */

#if CORE == CORE_LOGGING
void log_event_print_alterable_para_press_by_write_uart_fifo(uint32_t log_header, uint32_t presspara,
                                                             uint32_t paranum, ...)
{
#ifndef CONFIG_LOG_USE_DIAG_FRAME
    uint8_t uc_para_num;
    uint8_t uc_loop;
    uint32_t log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + OML_LOG_ALTER_PARA_MAX_NUM + OML_LOG_TAIL_LENTH];
    va_list args;
    uint32_t presspara_temp = presspara;
    if (log_check(&presspara_temp) == false) { return; }

    uc_para_num = (uint8_t)((paranum > OML_LOG_ALTER_PARA_MAX_NUM) ? (OML_LOG_ALTER_PARA_MAX_NUM) : paranum);

    log_oam_entry[LOG_OAM_INDEX_0] = log_header;
    log_oam_entry[LOG_OAM_INDEX_1] = (uint32_t)log_lenth_and_sn_press(uc_para_num, (uint32_t)get_log_sn_number());
    log_oam_entry[LOG_OAM_INDEX_2] = presspara_temp;

    va_start(args, paranum);
    for (uc_loop = 0; uc_loop < uc_para_num; uc_loop++) {
        log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + uc_loop] = (uint32_t)va_arg(args, uint32_t);
    }
    va_end(args);
    log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + uc_loop] = OM_FRAME_DELIMITER;
#ifdef CFG_DRIVERS_USB_AS_SERIAL
    uapi_usb_serial_write(USB_SERIAL_1, (const void *)log_oam_entry, oal_log_lenth(uc_para_num));
#else
    uapi_uart_write(LOG_UART_BUS, (const void *)log_oam_entry, oal_log_lenth(uc_para_num), 0);
#endif
#else
    unused(log_header);
    unused(presspara);
    unused(paranum);
#endif
}
#endif

#endif /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG */
