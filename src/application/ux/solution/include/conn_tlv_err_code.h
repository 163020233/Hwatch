/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: tlv err code
 * Author: AuthorNameMagicTag
 * Create: 2023-02-20
 */

#ifndef CONN_TLV_ERR_CODE_H
#define CONN_TLV_ERR_CODE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define CONN_TLV_OP_SUCC 0 // success
#define CONN_TLV_ERR_NO_MEM (-1) // no memory
#define CONN_TLV_ERR_PARAM (-2) // parameter err
#define CONN_TLV_ERR_LEN (-3) // len err
#define CONN_TLV_ERR_SMALL_BUF (-4) // buf not enough
#define CONN_TLV_ERR_SOURCE (-5) // memory operate err

#define CONN_TLV_ERR_TYPE               0xFF
#define ANC_DUT_ERROR                   0
#define ANC_DUT_SUCC                    1

typedef enum {
    CONN_APP_COM_SUCC = 100000,
    CONN_APP_COM_UNKNOWN_ERR = 100001,
    CONN_APP_COM_NONSUPT_SRV = 100002,
    CONN_APP_COM_NONSUPT_CMD = 100003,
    CONN_APP_COM_NO_PERMIT = 100004,
    CONN_APP_COM_SYS_BUSY = 100005,
    CONN_APP_COM_REQ_FMT_ERR = 100006,
    CONN_APP_COM_PARA_ERR = 100007,
    CONN_APP_COM_NO_MEM = 100008,
    CONN_APP_COM_RSP_TIMEOUT = 100009,
    CONN_APP_COM_LOW_BATTERY = 100010,
    CONN_APP_COM_DEV_WRONG = 100011,
    CONN_APP_COM_HAID_INVALID = 100012,

    CONN_APP_COM_ANC_DUT_FAIL = 100101,

    CONN_APP_COM_NOT_IN_CALL_STATE = 105001, /* 表示不在通话接听状态(不包括通话中音频通道切到听筒) */

    CONN_APP_COM_NO_RPT = 0x7FFFFFFF, // 表示主动上报或者查询接口不额外上报消息
} tlv_app_op_code_t;

typedef enum {
    TLV_HEARING_PARAM_ERR = 109001,
    TLV_HEARING_LEN_OVER = 109002
} tlv_haid_err_code_t;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
