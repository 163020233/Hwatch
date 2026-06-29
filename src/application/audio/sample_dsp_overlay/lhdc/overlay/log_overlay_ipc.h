/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2023. All rights reserved.
 * Description: head of overlay log for ipc
 * Author: audio
 */

#ifndef __LOG_OVERLAY_IPC_H__
#define __LOG_OVERLAY_IPC_H__

#include "audio_type.h"

/* HSO IPC LOG DEFINES begin */
#include "log_def_dsp.h"

#define OM_FRAME_DELIMITER                0x7E

#define OML_LOG_LEVEL_OFFSET              6
#define OML_LOG_LEVEL_MASK                0x3
#define OML_LOG_MODULE_MASK               0x3F
#define OM_LOG_HEAD_PRESS_MASK            0xFF
#define OM_MSG_TYPE_LOG                   1
#define OM_MSG_TYPE_LOG_OFFSET            8
#define OM_LOG_RPT_IND                    3
#define OM_LOG_RPT_IND_MODULE_ID_OFFSET   16
#define OM_LOG_SN_OFFSET                  16

enum oml_log_mod_id {
    LOG_MOD_WIFI    = 0,
    LOG_MOD_BT      = 1,
    LOG_MOD_GNSS    = 2,
    LOG_MOD_DSP     = 3,
    LOG_MOD_PF      = 4,
    LOG_MOD_RESERVE = 5,
    LOG_MOD_NFC     = 6,
    LOG_MOD_APP     = 7,
    LOG_MOD_MAX,
};

enum om_msg_mod_id {
    OM_WIFI_HOST = 0x00,
    OM_BT        = 0x10,
    OM_GNSS      = 0x20,
    OM_DSP       = 0x30,
    OM_PF        = 0x40,
    OM_WIFI      = 0x50,
    OM_NFC       = 0x60,
    OM_IR        = 0x70,
    OM_MAX,
};

typedef struct {
    td_u8 frame_start;
    td_u8 func_type;
    td_u8 prime_id;
    td_u8 arr_reserver[1];
    td_u16 frame_len;
    td_u16 sn;
} __attribute__((packed)) om_msg_header;

typedef enum {
    LOG_LEVEL_NONE,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DBG,
    LOG_LEVEL_MAX,
} log_level_e;

/* Log message struct */
typedef struct {
    om_msg_header header;
    td_u8 level;
    td_u8 file_id_high;
    td_u8 file_id_low : 2;
    td_u8 line_num_high : 6;
    td_u8 line_num_low;
} om_log_header_t;

#ifndef SAP_IPC_LOG_USE_DIAG_FRAME
#define log_head_press(log_mid) ((OM_FRAME_DELIMITER & OM_LOG_HEAD_PRESS_MASK) | \
                                   ((OM_MSG_TYPE_LOG & OM_LOG_HEAD_PRESS_MASK) << OM_MSG_TYPE_LOG_OFFSET) |  \
                                   ((OM_LOG_RPT_IND & OM_LOG_HEAD_PRESS_MASK) << OM_LOG_RPT_IND_MODULE_ID_OFFSET) |  \
                                   (((log_mid) & OM_LOG_HEAD_PRESS_MASK) << OM_LOG_RPT_IND_MODULE_ID_OFFSET))
/**
 * @brief  Fill this 32 bit value as little ending format
 * Last by will be send as the first byte by log system.
 * |----------------------------------------------------------------------------------------------------------------|
 * |-H-------------------------------------------------- 32 bit --------------------------------------------------L-|
 * |        8 bit       |                  8 bit                  |        8 bit       |            8 bit           |
 * | line_num low 8 bit | line_num high 6 bit + file_id low 2 bit | file_id high 8 bit | level 2 bit + mod_id 6 bit |
 * |----------------------------------------------------------------------------------------------------------------|
 */
#define para_press(mid, lev, fileid, lineno) (((td_u32)(((td_u32)(lineno) & 0xFF) << 0x18)) |  \
                                                  ((td_u32)((((td_u32)(fileid) & 0x3) << 0x10) |   \
                                                  ((td_u32)(lineno) & 0x3F00) << 0xA))  |          \
                                                  ((td_u32)(((td_u32)(fileid) & 0x3FC) << 0x6)) |  \
                                                  ((td_u32)(((mid) & 0x3F) | (((lev) & 0x3) << 0x6))))

#else
#define DIAG_LOG_LEVEL_ALERT    0
#define DIAG_LOG_LEVEL_FATAL    1
#define DIAG_LOG_LEVEL_ERROR    2
#define DIAG_LOG_LEVEL_WARN     3
#define DIAG_LOG_LEVEL_NOTICE   4
#define DIAG_LOG_LEVEL_INFO     5
#define DIAG_LOG_LEVEL_DBG      6
#define DIAG_LOG_LEVEL_TRACE    7

    /** @defgroup CHIP_Base CHIP Base Definitions
     * CHIP Base Definitions
     * @ingroup CHIP_Base
     * */
    /** @defgroup CHIP_ARMCORE CHIP ARM Core Specifics
     * CHIP ARM Core Application Core Specifics
     * @ingroup CHIP_Base
     * @{
     * */
#define BT       0
#define PROTOCOL 1
#define APPS     2
#define GNSS     3
#define SECURITY 4
#define HIFI     PROTOCOL
#define WIFI     5
#define CONTROL_CORE 6

#define CORE_ID HIFI

#if defined(HAVE_PCLINT_CHECK)
#define check_default_id(id) (id)
#else
#define check_default_id(id) (((id) == 0) ? __LINE__ : (id))
#endif

#define OAM_LOG_LEVEL_MASK              0x07
#define OAM_LOG_MOD_ID_MASK             0xF
#define OAM_LOG_MOD_ID_OFFSET           27
#ifndef OAM_LOG_PARAM_MAX_NUM
#define OAM_LOG_PARAM_MAX_NUM           10
#endif

    /** msg_id:
     * |-------------------------------------------------------------------------------------------------------|
     * |-H--------------------------------------------- 32 bit ----------------------------------------------L-|
     * |  1 bit(31)  |    4 bits(30 ~ 27)  |    10 bits(26 ~ 17)    |      14 bits(16 ~ 3)      | 3 bits(2 ~ 0)|
     * |  log_flg    |       mod_id        |        file_id         |          line No.         |    level     |
     * |-------------------------------------------------------------------------------------------------------|
     */
#define diag_log_msg_mk_id(mid, fileid, lineno, lv) (((td_u32)(1UL << 31)) \
        + (((td_u32)((mid) << 27)) & 0x78000000) + (((td_u32)((fileid) << 17)) & 0x7fe0000) \
        + ((((td_u32)(check_default_id(lineno))) << 3) & 0x1fff8) + ((lv) & 0x7))
#define diag_log_msg_mk_id_e(id) diag_log_msg_mk_id(LOG_MOD_DSP, THIS_FILE_ID, id, DIAG_LOG_LEVEL_ERROR)
#define diag_log_msg_mk_id_w(id) diag_log_msg_mk_id(LOG_MOD_DSP, THIS_FILE_ID, id, DIAG_LOG_LEVEL_WARN)
#define diag_log_msg_mk_id_n(id) diag_log_msg_mk_id(LOG_MOD_DSP, THIS_FILE_ID, id, DIAG_LOG_LEVEL_NOTICE)
#define diag_log_msg_mk_id_i(id) diag_log_msg_mk_id(LOG_MOD_DSP, THIS_FILE_ID, id, DIAG_LOG_LEVEL_INFO)
#define diag_log_msg_mk_id_d(id) diag_log_msg_mk_id(LOG_MOD_DSP, THIS_FILE_ID, id, DIAG_LOG_LEVEL_DBG)

    /** mod_id:
     * |----------------------------------------------------------------------------|
     * |-H-------------------------------- 16 bit --------------------------------L-|
     * |   3 bits(15 ~ 13)   |    5 bits(12 ~ 8)   |          8 bits(7 ~ 0)         |
     * |       core_id       |      sub_mod_id     |             mod_id             |
     * |----------------------------------------------------------------------------|
     */
#define diag_log_msg_mk_mod_id_custom(mod_id, sub_mod, core_id) ((td_u16)((td_u16)(mod_id) | \
        (((td_u16)(sub_mod) & 0x1f) << 8) | ((td_u16)(core_id) << 13)))
#define diag_log_msg_mk_mod_id(sub_mod) diag_log_msg_mk_mod_id_custom(LOG_MOD_DSP, sub_mod, CORE_ID)

#define log_head_press(log_mid) diag_log_msg_mk_mod_id(0)
#define para_press(mid, lev, fileid, lineno) diag_log_msg_mk_id(mid, fileid, lineno, lev)
#endif /* SAP_IPC_LOG_USE_DIAG_FRAME */

#define oml_dsp_log_print0(level, fmt) \
    log_event_print0(log_head_press(OM_DSP), para_press(LOG_MOD_DSP, level, THIS_FILE_ID, __LINE__))
#define oml_dsp_log_print1(level, fmt, p1) \
    log_event_print1(log_head_press(OM_DSP), para_press(LOG_MOD_DSP, level, THIS_FILE_ID, __LINE__), (td_u32)(p1))
#define oml_dsp_log_print2(level, fmt, p1, p2) \
    log_event_print2(log_head_press(OM_DSP), para_press(LOG_MOD_DSP, level, THIS_FILE_ID, __LINE__), (td_u32)(p1), \
                     (td_u32)(p2))
#define oml_dsp_log_print3(level, fmt, p1, p2, p3) \
    log_event_print3(log_head_press(OM_DSP), para_press(LOG_MOD_DSP, level, THIS_FILE_ID, __LINE__), (td_u32)(p1), \
                     (td_u32)(p2), (td_u32)(p3))
#define oml_dsp_log_print4(level, fmt, p1, p2, p3, p4) \
    log_event_print4(log_head_press(OM_DSP), para_press(LOG_MOD_DSP, level, THIS_FILE_ID, __LINE__), (td_u32)(p1), \
                     (td_u32)(p2), (td_u32)(p3), (td_u32)(p4))
/* HSO IPC LOG DEFINES end */

#define AUDIO_LOG_LEVEL_ALERT LOG_LEVEL_WARNING
#define AUDIO_LOG_LEVEL_FATAL LOG_LEVEL_ERROR
#define AUDIO_LOG_LEVEL_ERROR LOG_LEVEL_ERROR
#define AUDIO_LOG_LEVEL_WARN  LOG_LEVEL_WARNING
#define AUDIO_LOG_LEVEL_INFO  LOG_LEVEL_INFO
#define AUDIO_LOG_LEVEL_DBG   LOG_LEVEL_INFO
#define AUDIO_LOG_LEVEL_TRACE LOG_LEVEL_INFO

#define audio_log0(level, fmt)                 oml_dsp_log_print0(level, fmt)
#define audio_log1(level, fmt, p1)             oml_dsp_log_print1(level, fmt, p1)
#define audio_log2(level, fmt, p1, p2)         oml_dsp_log_print2(level, fmt, p1, p2)
#define audio_log3(level, fmt, p1, p2, p3)     oml_dsp_log_print3(level, fmt, p1, p2, p3)
#define audio_log4(level, fmt, p1, p2, p3, p4) oml_dsp_log_print4(level, fmt, p1, p2, p3, p4)

#define audio_func_enter() audio_log0(AUDIO_LOG_LEVEL_TRACE, "enter...")
#define audio_func_exit()  audio_log0(AUDIO_LOG_LEVEL_TRACE, "exit...")

#define audio_log_err(fmt...)

/* function trace log, strictly prohibited to expand */
#define sap_err_log_ret(err_code) audio_log1(AUDIO_LOG_LEVEL_ERROR, "Error Code: [0x%08X]\n", (td_u32)(err_code))
/* function trace log, print the called function name when function is error */
#define sap_err_log_fun(func, err_code)                                                                    \
    do {                                                                                                   \
        audio_log1(AUDIO_LOG_LEVEL_ERROR, "Call func Failed, Error Code: [0x%08X]\n", (td_u32)(err_code)); \
    } while (0)
/* Function trace log, print the pointer name when pointer is null */
#define sap_err_log_null_pointer(val) audio_log1(AUDIO_LOG_LEVEL_ERROR, "val = %p, Null Pointer!\n",  val)

/* Used for displaying more detailed error information */
#define sap_err_log_s32(val)   audio_log1(AUDIO_LOG_LEVEL_ERROR, "val = %d\n", val)
#define sap_err_log_u32(val)   audio_log1(AUDIO_LOG_LEVEL_ERROR, "val = %u\n", val)
#define sap_err_log_s64(val)
#define sap_err_log_u64(val)
#define sap_err_log_h32(val)   audio_log1(AUDIO_LOG_LEVEL_ERROR, "val = 0x%08X\n", (td_u32)(val))
#define sap_err_log_h64(val)
#define sap_err_log_str(val)
#define sap_err_log_void(val)  audio_log1(AUDIO_LOG_LEVEL_ERROR, "val = %p\n", val)
#define sap_err_log_float(val) audio_log1(AUDIO_LOG_LEVEL_ERROR, "val = %f\n", val)
#define sap_err_log_bool(val)  audio_log1(AUDIO_LOG_LEVEL_ERROR, "val = %d\n", val)
#define sap_err_log_info(val)  audio_log0(AUDIO_LOG_LEVEL_ERROR, "val")

#endif /* __LOG_OVERLAY_IPC_H__ */
