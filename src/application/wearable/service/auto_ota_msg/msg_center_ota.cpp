/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ota upgrade handle.
 * This file should be changed only infrequently and with great care.
 */
#include "build_version.h"
#include "securec.h"
#include "soc_osal.h"
#include "diag_service.h"
#include "upg.h"
#include "upg_common_porting.h"
#include "upg_definitions.h"
#include "diag_common.h"
#include "transmit.h"
#include "msg_center_ota.h"
#include "msg_center_protocol.h"
#include "wearable_log.h"
#include "upg_porting.h"
#ifdef SUPPORT_AUTO_OTA
#include "auto_ota_service.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SUPPORT_AUTO_OTA
extern osSemaphoreId_t g_auto_ota_transmit_done_sem;
extern uint8_t g_auto_ota_mac_tmp[];
#endif
#ifndef unused
#define unused(var)     (void)(var)
#endif

#define AUTO_OTA_UPG_PATH "/user/update/update.fwpkg"
#define AUTO_OTA_TRANS_BLOCK_SIZE (0x3B6) // 0x3B6: spp传输净荷使用950字节

static errcode_t msg_center_auto_ota_result_cb(errcode_t result, uintptr_t usr_data)
{
    unused(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    /* 目的端升级路径，建议固定 */
    uint32_t package_len = 0;
    const char *file_name = AUTO_OTA_UPG_PATH;
    FILE *fp = fopen(file_name, "rb");
    if (fp == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "open %s fail.\r\n", file_name);
        return ERRCODE_FAIL;
    }
    fseek(fp, 0L, SEEK_END);
    package_len = ftell(fp);
    fclose(fp);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "send file_len = %d\n", package_len);

    // 文件传输结果判断
    if (result == ERRCODE_SUCC) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "transmit success\n");
        // 文件传输成功，向Watch发送请求升级指令
        ret = msg_center_send_data(MSGCENTER_CMD_OTA, MSGCENTER_TYPE_ID_OTA_UPG_START, &package_len, sizeof(package_len));
        if (ret != ERRCODE_SUCC) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "send upgrade command fail, ret = %d\r\n", ret);
            return ERRCODE_FAIL;
        }
#ifdef SUPPORT_AUTO_OTA
        auto_ota_dev_add(g_auto_ota_mac_tmp);
        (void)osSemaphoreRelease(g_auto_ota_transmit_done_sem);
#endif
        return ret;
    } else {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "transmit fail\n");
#ifdef SUPPORT_AUTO_OTA
        (void)osSemaphoreRelease(g_auto_ota_transmit_done_sem);
#endif
        return ret;
    }
}

void msg_center_ota_upgrade_start(void)
{
    const char *file_name = AUTO_OTA_UPG_PATH;
    uint32_t total_size;
    errcode_t ret;
    FILE *fp = fopen(file_name, "rb");
    if (fp == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "open %s fail.\r\n", file_name);
        return;
    }
    fseek(fp, 0L, SEEK_END);
    total_size = ftell(fp);
    fclose(fp);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "read file size = %d\r\n", total_size);

    transmit_cfg_info_t cfg_info = {0};
    cfg_info.data.file_info.host_file_name = AUTO_OTA_UPG_PATH;
    cfg_info.data.file_info.device_file_name = AUTO_OTA_UPG_PATH;
    cfg_info.total_size = total_size;
    cfg_info.data_block_size = AUTO_OTA_TRANS_BLOCK_SIZE;
    cfg_info.re_transmit = false;
    transmit_callback_t callback = {0};
    callback.result_hook = msg_center_auto_ota_result_cb;
    ret = uapi_transmit_host_start(TRANSMIT_TYPE_FILE_DOWNSTREAM, DIAG_FRAME_FID_BT_SPP, &cfg_info, &callback);
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "uapi_transmit_host_start fail, ret = %x\r\n", ret);
        return;
    }

    return;
}

errcode_t msg_center_ota_rec_upg_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    /* 收到升级指令 */
    upg_prepare_info_t info;
    info.package_len = *(uint32_t *)tlv_payload;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "upg_prepare running......, rec file_len = %d\r\n", info.package_len);
    ret = uapi_upg_prepare(&info);
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "upg_prepare err, ret = 0x%x\r\n", ret);
        return ret;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "upg_prepare success!!!!\r\n");

    ret = uapi_upg_request_upgrade(false);
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "upg_request err, ret = 0x%x\r\n", ret);
        return ret;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "upg request success!!!!\r\n");

    ret = uapi_upg_start();
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "upg_request start, ret = 0x%x\r\n", ret);
        return ret;
    }
#ifdef UPG_DETECT_SSB_UPGREADE_IMAGE
    ret = upg_detect_upgrade_image();
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "upg_detect_upgrade_image, ret = 0x%x\r\n", ret);
        return ret;
    }
#endif
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "upg start success!!!!\r\n");

    upg_reboot();
    return ERRCODE_SUCC;
}

/* 客户逻辑实现 */
__attribute__((weak)) errcode_t msg_center_ota_upg_version_check(char *dst_ver, char *src_ver)
{
    unused(dst_ver);
    unused(src_ver);
    /* 客户版本号校验逻辑 */
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "upg version check default pass!\r\n");
    /* 客户检查本地剩余空间是否足够容纳升级包大小 */
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "upg space check default pass!\r\n");
    return ERRCODE_SUCC;
}

__attribute__((weak)) char * msg_center_ota_get_src_ver(void)
{
    char *src_ver = "XXX_YYY";
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "get src version : %s\r\n", src_ver);
    return src_ver;
}

errcode_t msg_center_ota_rec_upg_check(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    errcode_t res = ERRCODE_SUCC;
    uint8_t check_succ = 1;
    uint8_t check_fail = 0;

    char *dst_ver = (char *)malloc(payload_len + 1);
    if (dst_ver == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "rec_dst_version: malloc failed.\r\n");
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(dst_ver, payload_len, tlv_payload, payload_len);
    dst_ver[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "rec_dst_version: %s\r\n", dst_ver);

    ret = msg_center_ota_upg_version_check(dst_ver, msg_center_ota_get_src_ver());
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "upg version check fail, no need upg!\r\n");
        res = msg_center_send_data(cmd_id, MSGCENTER_TYPE_ID_OTA_UPG_CHECK_RESULT, &check_fail, sizeof(check_fail));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "send upg check fail, result = %d\r\n", res);
        free(dst_ver);
        dst_ver = NULL;
        return ret;
    }

    free(dst_ver);
    dst_ver = NULL;
    res = msg_center_send_data(cmd_id, MSGCENTER_TYPE_ID_OTA_UPG_CHECK_RESULT, &check_succ, sizeof(check_succ));
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "send upg check succ, result = %d\r\n", res);

    return ERRCODE_SUCC;
}

errcode_t msg_center_ota_rec_upg_check_result(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    uint8_t upg_check_result = *tlv_payload;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "upg_check_result = %d\r\n", upg_check_result);
    if (upg_check_result) {
        msg_center_ota_upgrade_start();
    } else {
#ifdef SUPPORT_AUTO_OTA
        (void)osSemaphoreRelease(g_auto_ota_transmit_done_sem);
#endif
    }

    return ERRCODE_SUCC;
}

#ifdef __cplusplus
}
#endif