/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag dfx
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_dfx_case.h"
#include "diag_dfx.h"
#include "panic.h"
#include "hal_reboot.h"
#include "diag.h"
#include "soc_diag_cmd_id.h"
#include "errcode.h"
#include "diag_log.h"
#include "soc_diag_msg_id.h"
#include "dfx_adapt_layer.h"
#include "diag_adapt_layer.h"
#include "log_module_id.h"
#include "last_dump.h"
#ifdef CONFIG_NV_FEATURE_SUPPORT
#include "nv_debug.h"
#endif
#if (defined(CONFIG_DFX_SUPPORT_TRANSMIT_FILE) && (CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES))
#include "transmit.h"
#endif
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES))
#include "massdata_buffer.h"
#include "massdata_reader.h"
#endif

#define CASE_PARAM_ZERO     0
#define CASE_PARAM_ONE      1
#define CASE_PARAM_TWO      2
#define MASS_DATA_MAX_LEN   128
#define TEST_SLEEP_TIME     10

STATIC errcode_t diag_dfx_report_sys_msg(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                         diag_option_t *option)
{
    uint32_t data[] = {1, 2, 3, 4}; /* test val 1 2 3 4 */
    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);

    uapi_diag_report_sys_msg(0, 0x1, (uint8_t*)data, sizeof(data), 1);  /* 0 0x1 1 test val */
    uapi_diag_report_sys_msg(1, 0xfeb40645, (uint8_t*)data, sizeof(data), 2); /* 1 0xfeb40645 2 test val */
    return ERRCODE_SUCC;
}

STATIC errcode_t diag_dfx_last_dump(uint16_t cmd_id, void * cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);

    diag_dfx_cmd_req_st_t *req = cmd_param;

    dfx_last_dump_data("dfx_dump", req->data[1], req->data[2]); /* 0 1 2 为data 下标 */
    return ERRCODE_SUCC;
}

STATIC errcode_t diag_dfx_cmd_get_stat(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                       diag_option_t *option)
{
    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);
#if defined(CONFIG_DFX_STATS) && (CONFIG_DFX_STATS == DFX_YES)
    return uapi_diag_report_packet(DIAG_CMD_ID_DIAG_DFX_START, option, (uint8_t *)uapi_diag_get_dfx_stat(),
        sizeof(diag_dfx_stat_t), true);
#else
    return ERRCODE_SUCC;
#endif
}

STATIC errcode_t diag_dfx_cmd_report_msg(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                         diag_option_t *option)
{
    uapi_diag_error_log(0, "test_error_log. p1 = %d", 1);
    uapi_diag_error_log1(1, "test_error_log1. p1 = %x", 3);    /* 1 3 test val */
    uapi_diag_warning_log(16, "test_warning_log p1 = %d", 5); /* 16 5 test val */
    uapi_diag_warning_log1(0, "test_warning_log1 p1 = %d", 2);   /* 2 test val */
    uapi_diag_info_log(0, "test_info_log p1 = 0x%x", 1);
    uapi_diag_info_log1(17, "test_info_log1 p1 = 0x%x", 4);   /* 17 4 test val */
    uapi_diag_debug_log(0, "test_debug_log p1 = %d", 6);         /* 6 test val */
    uapi_diag_debug_log1(0, "test_debug_log1 p1 = %d", 1);

    unused(cmd_id);
    unused(cmd_param_size);
    return uapi_diag_report_packet(cmd_id, option, (uint8_t *)cmd_param, sizeof(diag_dfx_cmd_req_st_t), false);
}

STATIC errcode_t diag_dfx_cmd_report_fix_msg(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                             diag_option_t *option)
{
    uint32_t data[] = {1, 2, 3, 4}; /* test val 1 2 3 4 */
    uapi_diag_report_sys_msg(THIS_MOD_ID, SOC_DIAG_MSG_ID_DIAG_TEST_U8_ARRAY, (uint8_t *)data, sizeof(data), 1);
    uapi_diag_report_sys_msg(THIS_MOD_ID, SOC_DIAG_MSG_ID_DIAG_TEST_U32_ARRAY, (uint8_t *)data, sizeof(data), 1);

    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);
    return ERRCODE_SUCC;
}

STATIC errcode_t diag_dfx_cmd_fault_mocked(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                           diag_option_t *option)
{
    unused(cmd_id);
    unused(cmd_param_size);
    unused(option);
    diag_dfx_cmd_req_st_t *req = cmd_param;

    if (req->data[0] == 0) {
#ifndef NDEBUG
        if (req->data[1] == 0) {
#ifndef UT_TEST
            uint32_t *null_pointer = NULL;
            *null_pointer = req->case_id;
#endif
        } else if (req->data[1] == 1) {    /* 1 mean fault type is circulation */
            while (true) {
#ifdef UT_TEST
                break;
#endif
            }
        } else if (req->data[1] == 2) {    /* 2 mean fault type is assert */
            dfx_assert(req->case_id != 5); /* fault mocked case id is 5 */
        } else if (req->data[1] == 3) {    /* 3 mean fault type is panic */
            panic(PANIC_LOG, __LINE__);
        }
#endif
    } else if (req->data[0] == 1) {
        hal_reboot_chip();
    }
    return ERRCODE_SUCC;
}


STATIC errcode_t diag_dfx_cmd_log_level(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                        diag_option_t *option)
{
    unused(cmd_param_size);
    diag_dfx_cmd_req_st_t *req = cmd_param;
    diag_dfx_cmd_ind_st_t ind = {0};

    if (req->data[0] == 0) {
        /* 查询 */
        ind.case_id = req->case_id;
        if (req->data[1] == 0) {
            ind.data[0] = diag_get_debug_level();
        } else if (req->data[1] == 1) {
#ifdef CONFIG_NV_FEATURE_SUPPORT
            ind.data[0] = nv_get_debug_level();
#endif
        }
        uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ind, sizeof(diag_dfx_cmd_ind_st_t), true);
    } else if (req->data[0] == 1) {
        /* 设置 */
        if (req->data[1] == 0) {
            /* DFX log level */
            diag_set_debug_level(req->data[2]); /* The level to be set is stored in data[2] */
            ind.data[0] = diag_get_debug_level();
        } else {
#ifdef CONFIG_NV_FEATURE_SUPPORT
            /* NV log level */
            nv_set_debug_level(req->data[2]); /* The level to be set is stored in data[2] */
            ind.data[0] = nv_get_debug_level();
#endif
        }
        ind.case_id = req->case_id;
        uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ind, sizeof(diag_dfx_cmd_ind_st_t), true);
    }
    return ERRCODE_SUCC;
}

STATIC errcode_t diag_dfx_cmd_register_stat(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                            diag_option_t *option)
{
    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);
#if defined(CONFIG_DFX_STATS) && (CONFIG_DFX_STATS == DFX_YES)
    diag_sys_stat_obj_t *obj = dfx_malloc(0, sizeof(diag_sys_stat_obj_t));
    diag_dfx_cmd_req_st_t *req = cmd_param;
    diag_dfx_cmd_ind_st_t ind = {0};
    errcode_t ret;
    if (obj != NULL) {
        obj->id = DIAG_CMD_ID_DIAG_DFX_START;
        obj->array_cnt = 1;
        obj->stat_packet_size = (uint32_t)sizeof(diag_dfx_stat_t);
        obj->stat_packet = (void *)uapi_diag_get_dfx_stat();
        ret = uapi_diag_register_stat_obj(obj, 1);
    } else {
        ret = ERRCODE_MALLOC;
    }

    ind.case_id = req->case_id;
    ind.data[0] = ret;
    uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ind, sizeof(diag_dfx_cmd_ind_st_t), true);
#endif
    return ERRCODE_SUCC;
}

#if (defined(CONFIG_DFX_SUPPORT_TRANSMIT_FILE) && (CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES))

STATIC errcode_t test_transmit_host_call_back(errcode_t result, uintptr_t usr_data)
{
    unused(result);
    unused(usr_data);
    dfx_log_err("test_transmit_host_call_back result = 0x%x usr_data = 0x%x\r\n", result, (uint32_t)usr_data);
    return ERRCODE_SUCC;
}

STATIC errcode_t test_transmit_device_call_back(errcode_t result, uintptr_t usr_data)
{
    unused(result);
    unused(usr_data);
    dfx_log_err("test_transmit_device_call_back result = 0x%x usr_data = 0x%x\r\n", result, (uint32_t)usr_data);
    return ERRCODE_SUCC;
}

STATIC errcode_t test_transmit_start_call_back(uint16_t channel_id, uintptr_t usr_data)
{
    unused(channel_id);
    unused(usr_data);
    dfx_log_err("test_transmit_start_call_back channel_id = 0x%x usr_data = 0x%x\r\n", channel_id, (uint32_t)usr_data);
    return ERRCODE_SUCC;
}

STATIC errcode_t diag_dfx_cmd_transmit(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_id);
    unused(cmd_param_size);
    unused(option);

    errcode_t ret = ERRCODE_SUCC;
    diag_dfx_cmd_req_st_t *req = cmd_param;
    diag_dfx_cmd_ind_st_t ind = {0};
    transmit_cfg_info_t cfg_info = {0};
    cfg_info.data.file_info.host_file_name = "/user/src/test_transmit.bin";
    cfg_info.data.file_info.device_file_name = "/user/dst/test_transmit.bin";
    cfg_info.total_size = 0x100000;
    cfg_info.re_transmit = false;

    transmit_callback_t callback = {0};
    callback.result_hook = test_transmit_host_call_back;
    callback.start_hook = test_transmit_start_call_back;
    if (req->data[0] == 0) {
        if (req->data[1] == 0) {
            ret = uapi_transmit_host_start(TRANSMIT_TYPE_FILE_DOWNSTREAM, DIAG_FRAME_FID_BT_SPP, &cfg_info, &callback);
        } else {
            ret = uapi_transmit_host_start(TRANSMIT_TYPE_FILE_UPSTREAM, DIAG_FRAME_FID_BT_SPP, &cfg_info, &callback);
        }
        dfx_log_err("uapi_transmit_host_start ret = 0x%x\r\n", ret);
    } else if (req->data[0] == 1) {
        if (req->data[1] == 0) {
            ret = uapi_transmit_host_stop(TRANSMIT_TYPE_FILE_DOWNSTREAM, DIAG_FRAME_FID_BT_SPP);
        } else {
            ret = uapi_transmit_host_stop(TRANSMIT_TYPE_FILE_UPSTREAM, DIAG_FRAME_FID_BT_SPP);
        }
        dfx_log_err("uapi_transmit_host_stop. ret = 0x%x\r\n", ret);
    } else if (req->data[0] == 2) { /* 2 means device stop cmd */
        ret = uapi_transmit_device_stop();
        dfx_log_err("uapi_transmit_device_stop.  ret = 0x%x\r\n", ret);
    } else if (req->data[0] == 3) { /* 3 means device register result hook cmd */
        transmit_callback_t device_callback = {0};
        if (req->data[1] == 0) {
            device_callback.result_hook = test_transmit_device_call_back;
            device_callback.start_hook = test_transmit_start_call_back;
            device_callback.result_usr_data = req->data[0];
            ret = uapi_transmit_device_register_result_hook(&device_callback);
        } else {
            ret = uapi_transmit_device_register_result_hook(&device_callback);
        }
    }

    ind.case_id = req->case_id;
    ind.data[0] = ret;
    uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ind, sizeof(diag_dfx_cmd_ind_st_t), true);
    return ret;
}
#endif

#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES))
STATIC errcode_t diag_dfx_cmd_massdata(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_id);
    unused(cmd_param_size);
    unused(option);

    diag_dfx_cmd_req_st_t *req = cmd_param;
    diag_dfx_cmd_ind_st_t ind = {0};

    if (req->data[CASE_PARAM_ZERO] == 0) {
        uapi_massdata_set_state((uint8_t)(req->data[CASE_PARAM_ONE]));
    } else if (req->data[CASE_PARAM_ZERO] == 1) {
        uapi_massdata_record_system_event(0, (uint8_t)req->data[CASE_PARAM_ONE], (uint8_t)req->data[CASE_PARAM_TWO], 1);
    } else if (req->data[CASE_PARAM_ZERO] == 2) { /* 2 means test system error */
        uapi_massdata_record_system_error(1, (uint8_t)req->data[CASE_PARAM_ONE], (uint8_t)req->data[CASE_PARAM_TWO], 1);
    } else if (req->data[CASE_PARAM_ZERO] == 3) { /* 3 means test system event Multiple times */
        uint32_t i;
        for (i = 0; i < req->data[CASE_PARAM_ONE]; i++) {
            uapi_massdata_record_system_event(0, (uint8_t)req->data[CASE_PARAM_TWO], (uint8_t)i, 0);
            dfx_sleep(TEST_SLEEP_TIME);
        }
    } else {
        uint32_t event_enum = 0;
        uint8_t info3[MASS_DATA_MAX_LEN] = {0};
        for (uint8_t i = 0; i < MASS_DATA_MAX_LEN; i++) {
            info3[i] = i;
        }
        for (uint32_t i = 0; i < req->data[CASE_PARAM_ONE]; i++) {
            uint8_t info_len = (uint8_t)(diag_adapt_get_msg_time_ms() % MASS_DATA_MAX_LEN);
            event_enum = joinu32(i, req->data[CASE_PARAM_TWO], 1U, 0);
            uapi_massdata_record_long_event(event_enum, info3, info_len);
            dfx_sleep(TEST_SLEEP_TIME);
        }
    }

    ind.case_id = req->case_id;
    ind.data[0] = 0;
    uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ind, sizeof(diag_dfx_cmd_ind_st_t), true);
    return ERRCODE_SUCC;
}
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE */

errcode_t diag_dfx_cmd(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    diag_dfx_cmd_req_st_t *req = cmd_param;
    unused(cmd_param_size);
    switch (req->case_id) {
        case DIAG_DFX_CMD_CASE_GET_STAT:
            return diag_dfx_cmd_get_stat(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_REPORT_MSG:
            return diag_dfx_cmd_report_msg(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_REPORT_FIX_MSG:
            return diag_dfx_cmd_report_fix_msg(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_LAST_DUMP:
            return diag_dfx_last_dump(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_SYS_MSG:
            return diag_dfx_report_sys_msg(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_FAULT_MOCKED:
            return diag_dfx_cmd_fault_mocked(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_SET_LOG_LEVEL:
            return diag_dfx_cmd_log_level(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_REGISTER_STAT:
            return diag_dfx_cmd_register_stat(cmd_id, cmd_param, cmd_param_size, option);
#if (defined(CONFIG_DFX_SUPPORT_TRANSMIT_FILE) && (CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES))
        case DIAG_DFX_CMD_CASE_TRANSMIT:
            return diag_dfx_cmd_transmit(cmd_id, cmd_param, cmd_param_size, option);
#endif /* CONFIG_DFX_SUPPORT_TRANSMIT_FILE */
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES))
        case DIAG_DFX_CMD_CASE_MASSDATA:
            return diag_dfx_cmd_massdata(cmd_id, cmd_param, cmd_param_size, option);
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE */
        default:
            return uapi_diag_report_packet(cmd_id, option, (uint8_t *)req, sizeof(diag_dfx_cmd_req_st_t), false);
    }
    return ERRCODE_SUCC;
}
