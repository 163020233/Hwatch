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

void uapi_esim_get_api_version(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "get_esim_api_version\r\n");
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "%s", api_version());
}

void uapi_esim_get_eid(void)
{
    int32_t res = 0;
    char eid[100];
    char *msg = (char *)malloc(sizeof(char) * 1024);
    if (msg == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "esim get eid mem malloc fail\r\n");
        return;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "get_eid\r\n");
    int eid_len = 100;
    int msg_len = 1024;
    res = esim_api_getEid(eid, eid_len, msg, msg_len);
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:getEid fail!\n %s \n", msg);
        free(msg);
        return;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "eid: %s \n", eid);
    free(msg);
}

void uapi_esim_get_product_edition(void)
{
    int32_t res = 0;
    char productEdition[100];
    char *msg = (char *)malloc(sizeof(char) * 1024);
    if (msg == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "msg == NULL \r\n");
        return;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "get_esim_product_edition\r\n");
    int product_edition_len = 100;
    int msg_len = 1024;
    res = esim_api_esimProductEdition(productEdition, product_edition_len, msg, msg_len);
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:get esimProductEdition fail!res=%d!\n %s \n", res, msg);
        free(msg);
        return;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "productEdition: %s \n", productEdition);
    free(msg);
}

uint32_t uapi_esim_get_euicc(void)
{
    int32_t res = 0;
    struct API_EuiccInfo euiccInfo = {0};
    char *msg = (char *)malloc(sizeof(char) * 1024);
    if (msg == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "msg == NULL \r\n");
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "get_euicc\r\n");
    int msg_len = 1024;
    res = esim_api_getEuicc(&euiccInfo, msg, msg_len);
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:get api_getEuicc fail!res=%d!\n %s \n", res, msg);
        free(msg);
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE,
        "EID=%s, GSM_RSP_Version=%s, Profile_Version=%s\r\n",
        euiccInfo.EID,
        euiccInfo.GSM_RSP_Version,
        euiccInfo.Profile_Version);
    free(msg);
    return ERRCODE_SUCC;
}

void uapi_esim_get_info(void)
{
    uapi_esim_get_api_version();
    uapi_esim_get_product_edition();
    uapi_esim_get_eid();
    uapi_esim_get_euicc();
}