/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-12
 */

#include "lwip_public.h"
#include "esim_lpa.h"
#include "esim_bind.h"
#include "ril_interface.h"
#include "wearable_log.h"
#include "esim_msg.h"

uint32_t uapi_esim_mcu_get_status(void)
{
    int32_t res = uapi_get_esim_status();
    if (res != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_status FAIL!");
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_status SUCC!");
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_esim_write_ppk(void)
{
    int32_t res;
    res = uapi_esim_write_ppk();
    if (res != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_write_ppk FAIL!");
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_write_ppk SUCC!");
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_bind_card(void)
{
    int32_t res;
    res = uapi_esim_bind_card();
    if (res != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_bind_card FAIL!");
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_bind_card SUCC!");
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_verify_card(void)
{
    int32_t res;

    res = uapi_esim_verify_card();
    if (res != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_verify_card FAIL!");
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_verify_card SUCC!");
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_unlock_bind(void)
{
    int32_t res;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_unlock_bind\n");
    res = esim_unlock_bind();
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:mcu_esim_unlock_bind fail!res=%d!\n", res);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_bind_main(void)
{
    int32_t res;
    res = uapi_esim_bind_main();
    if (res != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_bind_main FAIL!");
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_bind_main SUCC!");
    return ERRCODE_SUCC;
}
