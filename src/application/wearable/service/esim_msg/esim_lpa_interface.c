/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-12
 */

#include "lwip_public.h"
#include "esim_lpa.h"
#include "esim_bind.h"
#include "ril_interface.h"
#include "wearable_log.h"
#include "errcode.h"
#include "esim_msg.h"

/* @brief http request buffer */
#define ESIM_REQ_BUF_SIZE 2048
static char *g_esim_http_req_buf = NULL;

/* @brief http response buffer */
#define ESIM_RSP_BUF_SIZE 50000
static char *g_esim_http_rsp_buf = NULL;

#define ESIM_RX_BUF_SIZE 25000
static unsigned char *g_esim_rx_buf = NULL;

static uint32_t esim_set_interface_init(void)
{
    int32_t res;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "esim_set_interface_init \r\n");
    res = api_seInterfaceInit(esim_apdu_transmit_recv, esim_waitfor_ready);
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:api_seInterfaceInit fail!res=%d!\n", res);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static uint32_t esim_http_interface_init(void)
{
    int32_t res;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "esim_http_interface_init\r\n");
    res = api_httpInterfaceInit(esim_http_hostreq);
    if (res != 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:api_httpInterfaceInit fail!res=%d!\n", res);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static uint32_t esim_setup_imei(char *imei, int imei_len)
{
    int32_t res;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "esim_setup_imei:imei=%s \r\n", imei);
    res = esim_api_setupIMEI(imei, imei_len);
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:api_setupIMEI fail!res=%d!\n", res);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static uint32_t esim_set_matedata_init(void)
{
    int32_t res;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "esim_set_matedata_init \r\n");
    res = api_matedataRegisterCallback(esim_metadata_process);
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:esim_set_matedata_init fail!res=%d!\n", res);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

void uapi_esim_init(void)
{
    uint32_t res = 0;
    char imei[IMEI_LENGTH] = {0};
    if (esim_get_imei(imei, IMEI_LENGTH) != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "get imei fail\n");
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "imei:%s\n", imei);
    res += esim_apdu_transmit_init();
    res += esim_set_interface_init();
    res += esim_http_interface_init();
    res += esim_setup_imei(imei, IMEI_LENGTH);
    res += esim_set_matedata_init();
    if (res != 0) {
        esim_apdu_transmit_deinit();
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:mcu_esim_init fail!res=%d!\n", res);
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_init SUCCESS!\n\n");
}

static uint32_t esim_get_profile_class_list(
    struct API_Profile pList[], int32_t pageSize, int32_t pageNum, int32_t *count, int32_t profileClass, char *msg)
{
    int32_t res;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE,
        "esim_get_profile_class_list:pageSize=%d,pageNum=%d,profileClass=%d \r\n",
        pageSize,
        pageNum,
        profileClass);
    res = api_getProfileClassList(pList, pageSize, pageNum, count, profileClass, msg);
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:api_getProfileClassList fail!res=%d!\n %s \n", res, msg);
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "getProfileClassList SUCCESS!count=%d \r\n", *count);
    return ERRCODE_SUCC;
}

static uint32_t esim_get_profile_list(
    struct API_Profile pList[], int32_t pageSize, int32_t pageNum, int32_t *count, char *msg)
{
    int32_t res;

    WEARABLE_LOGI(
        WEARABLE_LOG_MODULE_MAIN_SERVICE, "esim_get_profile_list:pageSize=%d,pageNum=%d \r\n", pageSize, pageNum);
    res = api_getProfileList(pList, pageSize, pageNum, count, msg);
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:api_getProfileList fail!res=%d!\n %s \n", res, msg);
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "getProfileList SUCCESS!count=%d \r\n", *count);
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_get_profile_list(uint8_t *para, uint32_t para_len)
{
    struct API_Profile pList[10] = {0};
    int32_t count = 0;
    char *ptr = (char *)para;
    int32_t res = 0;
    int32_t pageSize = 10;
    int32_t pageNum = 0;
    int32_t profileClass, cnt;
    char *token = NULL;
    char *next_token = NULL;
    uint32_t idx = 0;
    uint32_t temp[3] = {0};  // 3：参数长度为3
    char *msg = (char *)malloc(sizeof(char) * 1024);
    if (msg == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "msg == NULL \r\n");
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_get_profile_list:para_len=%d\r\n", para_len);
    token = strtok_s((char *)ptr, ",", &next_token);
    while (token != NULL) {
        temp[idx++] = strtoul(token, NULL, 10);  // 10: 十进制
        token = strtok_s(NULL, ",", &next_token);
    }
    if (idx > 1) {
        pageSize = temp[0];
        pageNum = temp[1];
    }
    if (idx > 2) {               // 2:idx大小
        profileClass = temp[2];  // 2:idx位置
        res = esim_get_profile_class_list(pList, pageSize, pageNum, &count, profileClass, msg);
    } else {
        res = esim_get_profile_list(pList, pageSize, pageNum, &count, msg);
    }
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:mcu_esim_get_profile_list fail!res=%d!\n", res);
        free(msg);
        return ERRCODE_FAIL;
    }
    if (count > 0) {
        for (cnt = 0; cnt < count; cnt++) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE,
                "pList[%d]:Provider=%s, ProfileName=%s, ICCID=%s, IsdpAid=%s, State=%s,NickName=%s \r\n",
                cnt,
                pList[cnt].Provider,
                pList[cnt].ProfileName,
                pList[cnt].ICCID,
                pList[cnt].IsdpAid,
                pList[cnt].State,
                pList[cnt].NickName);
        }
    }
    free(msg);
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_get_profile(uint8_t *para, uint32_t para_len)
{
    struct API_Profile profile;
    char iccid[ICCID_BUF_SIZE] = {0};
    char *ptr = (char *)para;
    int32_t res;
    char *msg = (char *)malloc(sizeof(char) * 1024);
    if (msg == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "msg == NULL \r\n");
        return ERRCODE_FAIL;
    }
    esim_get_iccid(iccid, ICCID_BUF_SIZE);
    if (para_len > 0) {
        if (memcpy_s(iccid, sizeof(iccid), ptr, sizeof(iccid)) != EOK) {
            free(msg);
            return ERRCODE_FAIL;
        }
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_get_profile:iccid:%s\r\n ", iccid);
    int msg_len = 1024;
    res = esim_api_getProfile(&profile, iccid, ICCID_BUF_SIZE, msg, msg_len);
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:esim_api_getProfile fail!res=%d!\n %s \n", res, msg);
        free(msg);
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE,
        "Provider=%s, ProfileName=%s, ICCID=%s, IsdpAid=%s, State=%s,NickName=%s \r\n",
        profile.Provider,
        profile.ProfileName,
        profile.ICCID,
        profile.IsdpAid,
        profile.State,
        profile.NickName);
    free(msg);
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_edit_profile(uint8_t *para, uint32_t para_len)
{
    char *ptr = (char *)para;
    char iccid[ICCID_BUF_SIZE] = {0};
    char nickname[128] = {0};
    int32_t res;
    char *msg = NULL;
    if (para_len == 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "para_len == 0 \r\n");
        return ERRCODE_FAIL;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_edit_profile\r\n");
    esim_get_iccid(iccid, ICCID_BUF_SIZE);
    if (memcpy_s(nickname, sizeof(nickname), ptr, sizeof(nickname)) != EOK) {
        return ERRCODE_FAIL;
    }
    msg = (char *)malloc(sizeof(char) * 1024);  // 1024:1k大小
    if (msg == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "msg == NULL \r\n");
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "iccid:%s \r\nnickname:%s \r\n", iccid, nickname);
    int msg_len = 1024;
    res = esim_api_editProfile(iccid, ICCID_BUF_SIZE, nickname, sizeof(nickname), msg, msg_len);
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:api_editProfile fail!res=%d!\n%s\n", res, msg);
        free(msg);
        return ERRCODE_FAIL;
    }
    free(msg);
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_delete_profile(void)
{
    char iccid[ICCID_BUF_SIZE] = {0};
    int32_t res = 0;
    char *msg = (char *)malloc(sizeof(char) * 1024);
    char cnt = 0;
    if (msg == NULL) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "msg == NULL \r\n");
        return ERRCODE_FAIL;
    }
    esim_set_net_flag(false);
    esim_get_iccid(iccid, ICCID_BUF_SIZE);
    lwip_netif_switch_bt_pan();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_delete_profile:iccid:%s\r\n ", iccid);
    /* 循环获取状态，直至ready */
    while (true) {
        if ((esim_get_net_flag() == true) || (cnt > 120)) {  // 120:120周期，一个周期500ms，共60s
            break;
        }
        osDelay(500);  // 50:120周期，一个周期500ms，共60s
        cnt++;
    }
    int msg_len = 1024;
    res = esim_api_deleteProfile(iccid, ICCID_BUF_SIZE, msg, msg_len);
    esim_set_net_flag(false);
    lwip_netif_switch_volte();
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:api_deleteProfile fail!res=%d!\n%s\n", res, msg);
        free(msg);
        return ERRCODE_FAIL;
    }
    free(msg);
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_enable_profile(uint8_t *para, uint32_t para_len)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_enable_profiler\n");
    char *ptr = (char *)para;
    char iccid[ICCID_BUF_SIZE] = {0};
    int32_t refreshflag;
    int32_t res;
    char *msg = (char *)malloc(sizeof(char) * 1024);
    if (msg == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "msg == NULL \r\n");
        return ERRCODE_FAIL;
    }
    esim_get_iccid(iccid, ICCID_BUF_SIZE);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "iccid=%s! \r\n", iccid);
    if (para_len == 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "api_enableProfile: iccid=%s! \r\n", iccid);
        int msg_len = 1024;
        res = esim_api_enableProfile(iccid, ICCID_BUF_SIZE, msg, msg_len);
    } else {
        refreshflag = strtoul(ptr, NULL, 10);  // 10:十进制
        if (refreshflag > 1) {
            WEARABLE_LOGE(
                WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR: refreshflag=%d!It should be (0,1)! \r\n", refreshflag);
            free(msg);
            return ERRCODE_FAIL;
        }
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE,
            "api_enableProfileFlag: iccid=%s,refreshflag=%d! \r\n",
            iccid,
            refreshflag);
        res = api_enableProfileFlag(iccid, refreshflag, msg);
    }
    if (res != 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:mcu_esim_enable_profile fail!res=%d!\n%s\n", res, msg);
        free(msg);
        return ERRCODE_FAIL;
    }
    free(msg);
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_disable_profile(uint8_t *para, uint32_t para_len)
{
    char *ptr = (char *)para;
    char iccid[ICCID_BUF_SIZE] = {0};
    int32_t refreshflag;
    int32_t res;
    char *msg = (char *)malloc(sizeof(char) * 1024);
    if (msg == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "msg == NULL \r\n");
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_disable_profile\r\n");
    esim_get_iccid(iccid, ICCID_BUF_SIZE);
    if (para_len == 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "api_disableProfile: iccid=%s! \r\n", iccid);
        int msg_len = 1024;
        res = esim_api_disableProfile(iccid, ICCID_BUF_SIZE, msg, msg_len);
    } else {
        refreshflag = strtoul(ptr, NULL, 10);  // 10:十进制
        if (refreshflag > 1) {
            WEARABLE_LOGE(
                WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR: refreshflag=%d!It should be (0,1)! \r\n", refreshflag);
            free(msg);
            return ERRCODE_FAIL;
        }
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE,
            "api_disableProfileFlag: iccid=%s,refreshflag=%d! \r\n",
            iccid,
            refreshflag);
        res = api_disableProfileFlag(iccid, refreshflag, msg);
    }
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:mcu_esim_enable_profile fail!res=%d!\n%s\n", res, msg);
        free(msg);
        return ERRCODE_FAIL;
    }
    free(msg);
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_notification(void)
{
    int32_t res;
    char *msg = (char *)malloc(sizeof(char) * 1024);
    if (msg == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "msg == NULL \r\n");
        return ERRCODE_FAIL;
    }
    int msg_len = 1024;
    res = esim_api_rerunNotification(msg, msg_len);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_notification\r\n");
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:api_rerunNotification fail!res=%d!\n%s\n", res, msg);
        free(msg);
        return ERRCODE_FAIL;
    }
    free(msg);
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_download(void)
{
    int32_t res;
    char activeCode[ACTIVE_CODE_BUF_SIZE] = {0};
    char confirmCode[] = "123123";
    char *msg = (char *)malloc(sizeof(char) * 1024);
    char cnt = 0;
    if (msg == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "msg == NULL \r\n");
        return ERRCODE_FAIL;
    }
    esim_set_net_flag(false);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_download\r\n");
    lwip_netif_switch_bt_pan();
    esim_get_activecode(activeCode, ACTIVE_CODE_BUF_SIZE);
    /* 循环获取状态，直至ready */
    while (true) {
        if ((esim_get_net_flag() == true) || (cnt > 120)) {  // 120:120周期，一个周期500ms，共60s
            break;
        }
        osDelay(500);  // 50:120周期，一个周期500ms，共60s
        cnt++;
    }
    int msg_len = 1024;
    res = esim_api_profileDownloadAC(activeCode, ACTIVE_CODE_BUF_SIZE, confirmCode, sizeof(confirmCode), msg, msg_len);
    esim_set_net_flag(false);
    lwip_netif_switch_volte();
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:mcu_esim_download fail!res=%d!\n%s\n", res, msg);
        free(msg);
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_download SUCCESS!\n\n");
    free(msg);
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_reset(void)
{
    char *msg = (char *)malloc(sizeof(char) * 1024);
    int32_t res;
    if (msg == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "msg == NULL \r\n");
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "mcu_esim_resetr\n");
    int msg_len = 1024;
    res = esim_api_memoryReset(msg, msg_len);
    if (res != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE, "ERROR:api_memoryReset fail!res=%d!\n%s\n", res, msg);
        free(msg);
        return ERRCODE_FAIL;
    }
    free(msg);
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_set_iccid(uint8_t *para, uint32_t para_len)
{
    char *ptr = (char *)para;
    int32_t res;

    if (esim_set_iccid(ptr, para_len) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    };
    return ERRCODE_SUCC;
}

uint32_t uapi_esim_mcu_set_active_code(uint8_t *para, uint32_t para_len)
{
    char *ptr = (char *)para;
    int32_t res;

    if (esim_set_activecode(ptr, para_len) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    };
    return ERRCODE_SUCC;
}