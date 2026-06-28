/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: esim \n
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wearable_log.h"
#include "at_cmd.h"
#include "at_esim_cmd_register.h"
#include "at_esim_cmd_table.h"
#include "common.h"
#include "nv.h"
#include "esim_msg.h"
#include "at_esim.h"

#define AT_ESIM_FUNC_NUM (sizeof(at_esim_cmd_parse_table) / sizeof(at_esim_cmd_parse_table[0]))

at_ret_t at_esim_mcu_get_info(void)
{
    uapi_esim_get_info();
    return AT_RET_OK;
}

// LPA 相关接口
at_ret_t at_esim_mcu_set_active_code(esim_at_param *args)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "esim param string: %s \r\n", args->parm1);
    uapi_esim_mcu_set_active_code((uint8_t *)args->parm1, strlen(args->parm1));
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_set_iccid(esim_at_param *args)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "esim param string: %s \r\n", args->parm1);
    uapi_esim_mcu_set_iccid((uint8_t *)args->parm1, strlen(args->parm1));
    return AT_RET_OK;
}

at_ret_t at_esim_init(void)
{
    uapi_esim_init();
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_reset(void)
{
    uapi_esim_mcu_reset();
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_download(void)
{
    uapi_esim_mcu_download();
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_notification(void)
{
    uapi_esim_mcu_notification();
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_disable_profile(esim_at_param *args)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "esim param string: %s \r\n", args->parm1);
    uapi_esim_mcu_disable_profile((uint8_t *)args->parm1, strlen(args->parm1));
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_enable_profile(esim_at_param *args)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "esim param string: %s \r\n", args->parm1);
    uapi_esim_mcu_enable_profile((uint8_t *)args->parm1, strlen(args->parm1));
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_delete_profile(void)
{
    uapi_esim_mcu_delete_profile();
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_edit_profile(esim_at_param *args)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "esim param string: %s \r\n", args->parm1);
    uapi_esim_mcu_edit_profile((uint8_t *)args->parm1, strlen(args->parm1));
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_get_profile(esim_at_param *args)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "esim param string: %s \r\n", args->parm1);
    uapi_esim_mcu_get_profile((uint8_t *)args->parm1, strlen(args->parm1));
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_get_profile_list(esim_at_param *args)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "esim param string: %s \r\n", args->parm1);
    uapi_esim_mcu_get_profile_list((uint8_t *)args->parm1, strlen(args->parm1));
    return AT_RET_OK;
}

// 机卡绑定相关接口
at_ret_t at_esim_mcu_get_status(void)
{
    uapi_esim_mcu_get_status();
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_esim_write_ppk(void)
{
    uapi_esim_mcu_esim_write_ppk();
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_bind_card(void)
{
    uapi_esim_mcu_bind_card();
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_verify_card(void)
{
    uapi_esim_mcu_verify_card();
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_unlock_bind(void)
{
    uapi_esim_mcu_unlock_bind();
    return AT_RET_OK;
}

at_ret_t at_esim_mcu_bind_main(void)
{
    uapi_esim_mcu_bind_main();
    return AT_RET_OK;
}

at_ret_t at_esim_nv_test(void)
{
    esim_bind_info_t *test_info;
    test_info = (esim_bind_info_t *)malloc(sizeof(esim_bind_info_t));
    memset_s(test_info, sizeof(esim_bind_info_t), 0, sizeof(esim_bind_info_t));
    memset_s(test_info->imei, IMEI_LENGTH, 0x5b, IMEI_LENGTH);
    memset_s(test_info->eid, ESIM_BIND_EID_LENGTH, 0x5a, ESIM_BIND_EID_LENGTH);
    memset_s(test_info->share_key, ESIM_BIND_SHARE_KEY_LENGTH, 0x5c, ESIM_BIND_SHARE_KEY_LENGTH);
    test_info->valid = 1;
    errcode_t ret = uapi_nv_write(NV_ID_SYS_ESIM_BIND_INFO, (uint8_t *)test_info, sizeof(esim_bind_info_t));
    free(test_info);
    esim_bind_info_t *bind_info;
    bind_info = (esim_bind_info_t *)malloc(sizeof(esim_bind_info_t));
    memset_s(bind_info, sizeof(esim_bind_info_t), 0, sizeof(esim_bind_info_t));
    uapi_esim_get_bind_info(bind_info);
    free(bind_info);
    return AT_RET_OK;
}

void los_at_esim_cmd_register(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "los_at_esim_cmd_register EXCUTE\r\n");
    uapi_at_esim_register_cmd(at_esim_cmd_parse_table, AT_ESIM_FUNC_NUM);
}
