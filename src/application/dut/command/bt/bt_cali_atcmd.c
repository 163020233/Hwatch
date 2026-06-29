/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: bt cali set atcmd source file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-22, Create file. \n
 */

#include "errcode.h"
#include "at.h"
#include "app_at_srv.h"
#include "dut.h"
#include "key_id.h"
#include "tcxo.h"
#include "nv.h"
#include "app_at_uart.h"
#include "clock_calibration.h"
#include "app_at_uart.h"
#include "bt_cali_atcmd.h"

#define BT_CALI_ATCMD_MAX_SIZE    64
#define BT_MAX_CALI_TIMES         255
#define DECIMAL                   10
#define CAIL_STR_MAX_LEN          40
#define BT_INT_TYPE_HEX           16

#define BT_CALI_CODE_VAL          3.9
#define BT_CALI_CONST_VAL         10

#define BT_CALI_SUCC_MAX_FREQ     2
#define BT_CALI_SUCC_MIN_FREQ     (-2)

typedef void (*bt_factory_freq_offset_req_callback)(bt_dut_freq_offset_param *offset_param);
errcode_t bt_factory_freq_offset_register_callback(bt_factory_freq_offset_req_callback func);
errcode_t bt_factory_auto_cali(uint16_t cmd, const void *data);

static int16_t get_freq_val(uint16_t freq)
{
    return ((int8_t)(freq / BT_CALI_CONST_VAL)) * BT_CALI_CODE_VAL;
}

static bt_auto_cali_check g_cali_check = {
    0, false, -1
};

static volatile bool g_is_golden_board = false;
static bd_addr g_ac_test_board_addr;
static bd_addr g_ac_golden_board_addr;
static volatile bt_auto_cali_mode_t g_auto_cali_mode = BT_AUTO_CALI_MODE_EFFICACIOUS;
static volatile bool g_is_only_query = false;

enum {
    BT_CALI_ATCMD_NUM_1,
    BT_CALI_ATCMD_NUM_2,
    BT_CALI_ATCMD_NUM_3,
    BT_CALI_ATCMD_NUM_4,
    BT_CALI_ATCMD_NUM_5,
    BT_CALI_ATCMD_NUM_MAX,
};

typedef struct {
    uint32_t                para_map;
    const char              *para1;
    const char              *para2;
    const char              *para3;
} bt_auto_cali_args_t;

static const at_para_parse_syntax_t g_bt_auto_cali_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute =  AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = CAIL_STR_MAX_LEN,
        .offset = offsetof(bt_auto_cali_args_t, para1),
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute =  AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.string.max_length = CAIL_STR_MAX_LEN,
        .offset = offsetof(bt_auto_cali_args_t, para2),
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute =  AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.string.max_length = CAIL_STR_MAX_LEN,
        .offset = offsetof(bt_auto_cali_args_t, para3),
    },
};

/*
    AT^BTAUTOCALI=GOLDEN,112233445566      golden board init
    AT^BTAUTOCALI=TEST,AABBCCDDEEFF        test board init
    AT^BTAUTOCALI=OPEN                     test board open auto cali
    AT^BTAUTOCALI=CLOSE                    test board close auto cali
    AT^BTAUTOCALI=START,0                  test board start efficacious auto cali
    AT^BTAUTOCALI=START,1                  test board start accuracy auto cali
    AT^BTAUTOCALI=QUERY                    test board query freq
    AT^BTAUTOCALI=CONNECT                  golden board connect
    AT^BTAUTOCALI=DISCONNECT               golden board disconnect, cali error
    AT^BTAUTOCALI?                         test board get cali data or connect info
    AT^BTAUTOCALI=GET,NV                   get NV intermediate frequency offset calibration value
    AT^BTAUTOCALI=SET,NV,111               set NV intermediate frequency offset calibration value
    AT^BTAUTOCALI=GET,REG                  get CTRIM reg intermediate frequency offset calibration value
    AT^BTAUTOCALI=SET,REG,111              set CTRIM reg intermediate frequency offset calibration value
*/
static at_ret_t bt_atcmd_auto_cali_ops(const void *args);
static at_ret_t bt_atcmd_auto_cali_start(const bt_auto_cali_args_t *args);

const at_cmd_entry_t g_bt_cali_atcmd_table[] = {
    {
        "BTAUTOCALI",
        BT_CALI_ATCMD_NUM_1,
        DUT_CMD_LEVEL_DUT,
        g_bt_auto_cali_syntax,
        NULL,
        bt_atcmd_auto_cali_ops,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL,
#endif
    },
};

void match_bt_addr(const char *para, bd_addr addr)
{
    char c[3]; /* buf len 3 for mac addr */
    int i = 0;
    int j = BD_ADDR_SIZE - 1;
    if (para == NULL) {
        return;
    }
    while (*para != '\0') {
        if (!((*para >= '0' && *para <= '9') || (*para >= 'a' && *para <= 'f') ||
            (*para >= 'A' && *para <= 'F'))) {
            para++;
            continue;
        }
        if ((i >= 0x0) && (i < 0x3)) {  /* 0x3 */
            c[i] = *para;
        }

        if (i == 1) {
            c[sizeof(c) - 1] = '\0'; /* index 2 is the last bype of arry c */
            if ((j >= 0) && (j <= BD_ADDR_SIZE - 1)) {
                addr[j] = (uint8_t)strtol(c, NULL, BT_INT_TYPE_HEX);
            }
            if (j == 0) {
                break;
            }
            j--;
            i = 0;
        } else {
            i = 1;
        }
        para++;
    }
    at_uart_printf("[AT][manu_test_case]src_addr = %x:%x:%x:%x:%x:%x\n",
        addr[0], addr[1],
        addr[2], addr[3],   /* indec 2 and 3 */
        addr[4], addr[5]);  /* indec 4 and 5 */
}

static at_ret_t bt_auto_cali_init_op(const bt_auto_cali_args_t *args)
{
    if (args->para2 == NULL) {
        at_uart_printf("\r\n^ERRINFO:NO BT ADDR\r\n");
    }

    if (strcasecmp(args->para1, "GOLDEN") == 0) {
        bt_offset_test_mode_info test_info = {0};
        test_info.role = 0;
        test_info.action = 1;
        g_is_golden_board = true;
        /* start connect */
        if (memset_s((void *)g_ac_test_board_addr, sizeof(g_ac_test_board_addr), 0,
                     sizeof(g_ac_test_board_addr)) != 0) {
            return AT_RET_SYNTAX_ERROR;
        }
        match_bt_addr(args->para2, g_ac_test_board_addr);
        errcode_t ret;
        ret = (errcode_t)bt_factory_auto_cali(BT_AUTO_CALI_CMD_ENTER_OFFSET_TEST_MODE, (void *)(&test_info));
        if (ret != ERRCODE_SUCC) {
            return AT_RET_SYNTAX_ERROR;
        }
    } else if (strcasecmp(args->para1, "TEST") == 0) {
        /* start connect */
        if (memset_s((void *)g_ac_golden_board_addr, sizeof(g_ac_golden_board_addr), 0,
                     sizeof(g_ac_golden_board_addr)) != 0) {
            return AT_RET_SYNTAX_ERROR;
        }
        match_bt_addr(args->para2, g_ac_golden_board_addr);
    }

    return AT_RET_OK;
}

static at_ret_t bt_auto_cali_open_op(const bt_auto_cali_args_t *args)
{
    bt_offset_test_mode_info test_info = {0};

    if (args->para2 != NULL) {
        return AT_RET_SYNTAX_ERROR;
    }

    if (g_is_golden_board) {
        at_uart_printf("\r\n^ERRINFO:NOT TEST BOARD\r\n");
        return AT_RET_SYNTAX_ERROR;
    }

    if (strcasecmp(args->para1, "CLOSE") == 0) {
        test_info.action = 0;
    } else if (strcasecmp(args->para1, "OPEN") == 0) {
        test_info.action = 1;
    }

    test_info.role = 1;
    errcode_t ret;
    ret = (errcode_t)bt_factory_auto_cali(BT_AUTO_CALI_CMD_ENTER_OFFSET_TEST_MODE, (void *)(&test_info));
    if (ret != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

static at_ret_t bt_auto_cali_connect_op(const bt_auto_cali_args_t *args)
{
    bt_dut_info dut_info = {0};
    if (!g_is_golden_board) {
        at_uart_printf("\r\n^ERRINFO:NOT GOLDEN BOARD\r\n");
        return AT_RET_SYNTAX_ERROR;
    }

    /* start connect */
    if (memcpy_s((dut_info.addr), sizeof(dut_info.addr), (void *)g_ac_test_board_addr,
                 sizeof(g_ac_test_board_addr)) != 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    if (strcasecmp(args->para1, "CONNECT") == 0) {
        dut_info.action = AUTO_CALI_CONNECT;
    } else if (strcasecmp(args->para1, "DISCONNECT") == 0) {
        dut_info.action = AUTO_CALI_DISCONNECT;
    }
    if (bt_factory_auto_cali(BT_AUTO_CALI_CMD_CONN_DUT, (void *)(&dut_info)) != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

static errcode_t bt_atcmd_auto_cali_get_freq(void)
{
    bt_dut_info dut_info = {0};

    if (memcpy_s((dut_info.addr), sizeof(dut_info.addr), (void *)g_ac_golden_board_addr,
                 sizeof(g_ac_golden_board_addr)) != 0) {
        return ERRCODE_FAIL;
    }
    dut_info.action = AUTO_CALI_DATA_REQUEST;
    if (bt_factory_auto_cali(BT_AUTO_CALI_CMD_CONN_DUT, (void *)(&dut_info)) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static at_ret_t bt_atcmd_auto_cali_start(const bt_auto_cali_args_t *args)
{
    if (args->para2 == NULL) {
        return AT_RET_CMD_PARA_ERROR;
    }

    if (g_is_golden_board) {
        at_uart_printf("\r\n^ERRINFO:NOT TEST BOARD\r\n");
        return AT_RET_SYNTAX_ERROR;
    }

    if (strcasecmp(args->para2, "1") == 0) {
        at_uart_printf("\r\n^BT_AUTO_CALI_MODE_ACCURACY\r\n");
        g_auto_cali_mode = BT_AUTO_CALI_MODE_ACCURACY;
    }

    if (bt_atcmd_auto_cali_get_freq() == ERRCODE_SUCC) {
        return AT_RET_OK;
    }
    return AT_RET_SYNTAX_ERROR;
}

static at_ret_t bt_atcmd_auto_cali_query(void)
{
    g_is_only_query = true;
    if (bt_atcmd_auto_cali_get_freq() == ERRCODE_SUCC) {
        return AT_RET_OK;
    }
    return AT_RET_SYNTAX_ERROR;
}

static errcode_t bt_atcmd_auto_cali_save_nv(uint16_t val)
{
    uint16_t xoval = val;

    nv_key_attr_t attr = { false, false, true, 0 };
    if (uapi_nv_write_with_attr(NV_ID_BTSRV_XOTRIM0_CONFIG, (uint8_t *)&(xoval),
        sizeof(uint16_t), &attr, NULL) != ERRCODE_SUCC) {
        at_uart_printf("\r\nERRINFO:CTRIM CALI SAVE NV FAILED\r\n");
        return ERRCODE_FAIL;
    }
    at_uart_printf("AUTO CALI TRIM:0x%x\r\n", xoval);
    return ERRCODE_SUCC;
}

static at_ret_t bt_atcmd_cali(const bt_auto_cali_args_t *args)
{
    static uint16_t xoval = 0;
    if (strcasecmp(args->para1, "SET") == 0 && args->para2 != NULL && args->para3 != NULL) {
        xoval = (uint16_t)strtol(args->para3, NULL, DECIMAL);
        if (strcasecmp(args->para2, "NV") == 0) {
            bt_atcmd_auto_cali_save_nv(xoval);
        } else if (strcasecmp(args->para2, "REG") == 0) {
            calibration_set_xo_core_ctrim(xoval);
        } else {
            return AT_RET_SYNTAX_ERROR;
        }
    } else if (strcasecmp(args->para1, "GET") == 0 && args->para2 != NULL) {
        if (strcasecmp(args->para2, "NV") == 0) {
            uint16_t val_len;
            errcode_t ret = uapi_nv_read(NV_ID_BTSRV_XOTRIM0_CONFIG, sizeof(uint16_t), &val_len, (uint8_t *)&xoval);
            if (ret != ERRCODE_SUCC) {
                at_uart_printf("cali get nv failed %d\r\n", ret);
                return AT_RET_SYNTAX_ERROR;
            }
            at_uart_printf("XOTRIM NV CONFIG: %d\r\n", xoval);
        } else if (strcasecmp(args->para2, "REG") == 0) {
#if defined(TARGET_CHIP_3322) && TARGET_CHIP_3322
            calibration_get_xo_core_ctrim_reg((uint8_t*)&xoval);
#else
            calibration_get_xo_core_ctrim_reg(&xoval);
#endif
            at_uart_printf("XOTRIM REG CONFIG: %d\r\n", xoval);
        } else {
            return AT_RET_SYNTAX_ERROR;
        }
    } else {
        return AT_RET_CMD_PARA_ERROR;
    }
    return AT_RET_OK;
}

static at_ret_t bt_atcmd_auto_cali_ops(const void *args)
{
    const bt_auto_cali_args_t *param = args;
    if (param == NULL || param->para1 == NULL) {
        return AT_RET_CMD_PARA_ERROR;
    }
    if (strcasecmp(param->para1, "GOLDEN") == 0 || strcasecmp(param->para1, "TEST") == 0) {
        return bt_auto_cali_init_op(param);
    } else if (strcasecmp(param->para1, "OPEN") == 0 || strcasecmp(param->para1, "CLOSE") == 0) {
        return bt_auto_cali_open_op(param);
    } else if (strcasecmp(param->para1, "CONNECT") == 0 || strcasecmp(param->para1, "DISCONNECT") == 0) {
        return bt_auto_cali_connect_op(param);
    } else if (strcasecmp(param->para1, "START") == 0) {
        return bt_atcmd_auto_cali_start(param);
    } else if (strcasecmp(param->para1, "QUERY") == 0) {
        return bt_atcmd_auto_cali_query();
    } else if (strcasecmp(param->para1, "SET") == 0 || strcasecmp(param->para1, "GET") == 0) {
        return bt_atcmd_cali(param);
    } else {
        at_uart_printf("error %s\r\n", param->para1);
    }
    return AT_RET_SYNTAX_ERROR;
}

static void bt_atcmd_auto_cali_xo(const char* mode)
{
    if (strcasecmp(mode, "+") == 0) {
        calibration_xo_core_ctrim_algorithm(true, 1);
    } else if (strcasecmp(mode, "-") == 0) {
        calibration_xo_core_ctrim_algorithm(false, 1);
    } else {
        at_uart_printf("\r\n^ERRINFO:ERROR AUTO CALI XO\r\n");
    }
}

static void bt_atcmd_auto_cali_check(int16_t freq_offset)
{
    if (abs(freq_offset) >= abs(g_cali_check.last_freq)) {
        if (g_cali_check.last_op == 0) {
            bt_atcmd_auto_cali_xo("-");
        }
        if (g_cali_check.last_op == 1) {
            bt_atcmd_auto_cali_xo("+");
        }
        at_uart_printf("FINAL FREQ IS %dKHz\r\n", -(int16_t)(g_cali_check.last_freq * BT_CALI_CODE_VAL));
    } else {
        at_uart_printf("FINAL FREQ IS %dKHz\r\n", -(int16_t)(freq_offset * BT_CALI_CODE_VAL));
    }
    g_cali_check.last_freq = 0;
    g_cali_check.last_op = -1;
    g_cali_check.is_final_check = false;

    uint16_t xoval = 0;
#if defined(TARGET_CHIP_3322) && TARGET_CHIP_3322
    calibration_get_xo_core_ctrim_reg((uint8_t*)&xoval);
#else
    calibration_get_xo_core_ctrim_reg(&xoval);
#endif
    if (bt_atcmd_auto_cali_save_nv(xoval) != ERRCODE_SUCC) {
        at_uart_printf("SAVE NV FALIED\r\n");
    }
}

static bt_auto_cali_result_t bt_atcmd_auto_cali(int16_t freq_offset)
{
    uint16_t xoval = 0;
    static uint16_t cali_times = 0;

    if (cali_times >= BT_MAX_CALI_TIMES) {
        cali_times = 0;
        return BT_AUTO_CALI_FAILED;
    }
#if defined(TARGET_CHIP_3322) && TARGET_CHIP_3322
    calibration_get_xo_core_ctrim_reg((uint8_t*)&xoval);
#else
    calibration_get_xo_core_ctrim_reg(&xoval);
#endif

    if (freq_offset >= BT_CALI_SUCC_MIN_FREQ && freq_offset <= BT_CALI_SUCC_MAX_FREQ) {
        at_uart_printf("INTEGER CALI TIMES:%d\r\n", cali_times);
        cali_times = 0;
        g_cali_check.last_freq = freq_offset;
        g_cali_check.is_final_check = true;
        if (freq_offset > 0) {
            bt_atcmd_auto_cali_xo("-");
            g_cali_check.last_op = 1;
            return BT_AUTO_CALI_CONTINUE;
        } else if (freq_offset < 0) {
            bt_atcmd_auto_cali_xo("+");
            g_cali_check.last_op = 0;
            return BT_AUTO_CALI_CONTINUE;
        }

        g_cali_check.is_final_check = false;
#if defined(TARGET_CHIP_3322) && TARGET_CHIP_3322
        calibration_get_xo_core_ctrim_reg((uint8_t*)&xoval);
#else
        calibration_get_xo_core_ctrim_reg(&xoval);
#endif
        if (bt_atcmd_auto_cali_save_nv(xoval) != ERRCODE_SUCC) {
            at_uart_printf("SAVE NV FALIED\r\n");
        }
        return BT_AUTO_CALI_SUCC;
    }

    cali_times++;

    if (freq_offset < BT_CALI_SUCC_MIN_FREQ) {
        bt_atcmd_auto_cali_xo("+");
    } else {
        bt_atcmd_auto_cali_xo("-");
    }

    return BT_AUTO_CALI_CONTINUE;
}

static void bt_device_handle_auto_cali_conn_resp(bt_dut_freq_offset_param *resp)
{
    bt_auto_cali_result_t bt_ret = BT_AUTO_CALI_FAILED;
    int8_t real_freq_offest = (int8_t)((resp->freq_offset) / 10);

    if (resp->conn_state == AUTO_CALI_DISCONNECT) {
        at_uart_printf("AUTO CALI DISCONNECTED,REASON 0x%x\r\n", resp->reason_code);
    } else if (resp->conn_state == AUTO_CALI_CONNECT) {
        if (g_is_golden_board) {
            at_uart_printf("AUTO CALI CONNECTED\r\n");
        }
        return;
    } else if (resp->conn_state == AUTO_CALI_DATA_REQUEST) {
        if (g_is_only_query) {
            at_uart_printf("QUERY FREQ IS %dKHz\r\n", -get_freq_val(resp->freq_offset));
            g_is_only_query = false;
            return;
        }
        if (g_cali_check.is_final_check) {
            bt_atcmd_auto_cali_check(real_freq_offest);
            at_uart_printf("AUTO CALI SUCCESS\r\n");
            return;
        }
        bt_ret = bt_atcmd_auto_cali(real_freq_offest);
        if (bt_ret == BT_AUTO_CALI_CONTINUE) {
            bt_atcmd_auto_cali_get_freq();
            return;
        } else if (bt_ret == BT_AUTO_CALI_FAILED) {
            at_uart_printf("AUTO CALI FAILED\r\n");
            return;
        }
        if (g_auto_cali_mode == BT_AUTO_CALI_MODE_ACCURACY && bt_ret != BT_AUTO_CALI_SUCC) {
            g_auto_cali_mode = BT_AUTO_CALI_MODE_EFFICACIOUS;
        }
        if (g_auto_cali_mode == BT_AUTO_CALI_MODE_EFFICACIOUS && bt_ret == BT_AUTO_CALI_SUCC) {
            at_uart_printf("AUTO CALI SUCCESS\r\n");
            return;
        }
    }
    at_uart_printf("AUTO CALI CONNECT RESPONSE ERROR\r\n");
    return;
}

errcode_t bt_cali_at_cmd_init(void)
{
    bt_factory_freq_offset_register_callback(bt_device_handle_auto_cali_conn_resp);
    return uapi_at_cmd_table_register(g_bt_cali_atcmd_table,
        sizeof(g_bt_cali_atcmd_table) / sizeof(at_cmd_entry_t), BT_CALI_ATCMD_MAX_SIZE);
}
