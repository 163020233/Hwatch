/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: gnss common proc
 * Author:
 * Create:
 */

#include "gnss_common_proc.h"
#include <stdbool.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include "gnss_device.h"
#include "gnss_log.h"
#include "gnss_factory_test.h"
#include "tcxo.h"
#include "gnss_log_mgr.h"
#include "gnss_common.h"

#define BUFF_MAX_SIZE               10240
#define CFG_BUFF_SIZE               100
#define GNSS_ENCODE_BUFF_MAX_LEN    9300
#define GNSS_NONE_STATE             0
#define GNSS_OPEN_STATE             1
#define GNSS_IDLE_STATE             2
#define GNSS_RUNNING_STATE          3
#define GNSS_DELAY_TIME             200    // 200毫秒
#define RTCM_DATA_BUFF_LEN 3000

static uint8_t g_gnssMsgBuff[BUFF_MAX_SIZE] = {0};
static bool g_isNeedSaveBinary = false;
static bool g_isNeedSaveNmea = false;
static bool g_isNeedPrintNmea = true;
static uint32_t g_gnssState = GNSS_NONE_STATE;
static long g_cur_pos = 0;
static FILE *g_rtcm_file = NULL;

void update_gnss_nmea_print_flag(uint8_t flag)
{
    if (flag != 0) {
        g_isNeedPrintNmea = true;
    } else {
        g_isNeedPrintNmea = false;
    }
}

void update_binary_save_flag(uint8_t flag)
{
    if (flag != 0) {
        g_isNeedSaveBinary = true;
    } else {
        g_isNeedSaveBinary = false;
    }
}

void update_nmea_save_flag(uint8_t flag)
{
    if (flag != 0) {
        g_isNeedSaveNmea = true;
    } else {
        g_isNeedSaveNmea = false;
    }
}

void gnss_proc_binary_msg(uint8_t *buff, uint32_t len)
{
    errno_t ret = memcpy_s(g_gnssMsgBuff, BUFF_MAX_SIZE, buff, len);
    if (ret != EOK) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss_proc_binary_msg memcpy fail\n");
        return;
    }
    if (g_isNeedSaveBinary) {
        gnss_save_data(g_gnssMsgBuff, len);
    }
    GnssMsg *msg = (GnssMsg*)g_gnssMsgBuff;
    if (msg == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss data is null");
        return;
    }

    if (msg->cmd == GNSS_FACTORY_TEST_RESULT) {
        fac_proc_data(g_gnssMsgBuff);
    }
}

void inject_rtcm_data(uint32_t target_second)
{
    while (feof(g_rtcm_file) == 0) {
        uint32_t second;
        uint32_t size;
        uint8_t buff[RTCM_DATA_BUFF_LEN] = {0};
        (void)fread(&second, sizeof(second), 1, g_rtcm_file);
        if (second > target_second) {
            fseek(g_rtcm_file, g_cur_pos, SEEK_SET);
            break;
        }
        (void)fread(&size, sizeof(size), 1, g_rtcm_file);
        (void)fread(buff, 1, size, g_rtcm_file);
        g_cur_pos = ftell(g_rtcm_file);
        GNSS_LOG_INFO("get rtcm msg from file, %d, %d, %ld\n", target_second % 1024, second % 1024, size);
        // uapi_gnss_set_rtcm_data(buff, size);
    };
}

void gnss_proc_nmea_msg(uint8_t *buff, uint32_t len)
{
    errno_t ret = memcpy_s(g_gnssMsgBuff, BUFF_MAX_SIZE, buff, len);
    if (ret != EOK) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss_proc_nmea_msg memcpy fail\n");
        return;
    }
    if (g_isNeedSaveNmea) {
        gnss_save_data(g_gnssMsgBuff, len);
    }
    if (g_gnssState == GNSS_OPEN_STATE || !g_isNeedPrintNmea) {
        return;
    }
    g_gnssMsgBuff[len] = '\0';
    GNSS_LOG_INFO("%s\n", (char *)g_gnssMsgBuff);
}

errcode_t gnss_init(void)
{
    if (g_gnssState != GNSS_NONE_STATE) {
        GNSS_LOG_WARNING("[GNSS WARNING] gnss already init, just return!\n");
        return ERRCODE_FAIL;
    }
    errcode_t ret = uapi_gnss_power_on();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss power on fail, error code: %#x\n", ret);
        return ret;
    }

    ret = uapi_gnss_init();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss init fail, error code: %#x\n", ret);
        return ret;
    }

    init_gnss_log_ctrl_info();

    ret = uapi_gnss_register_callback(GNSS_CALLBACK_MSG, gnss_proc_binary_msg);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("Failed to register the processing binary callback, error code: %#x\n", ret);
        return ret;
    }
    ret = uapi_gnss_register_callback(GNSS_CALLBACK_NMEA, gnss_proc_nmea_msg);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]Failed to register the processing nmea callback, error code: %#x\n", ret);
        return ret;
    }
    ret = uapi_gnss_open();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss open fail, error code: %#x\n", ret);
        return ret;
    }
    g_rtcm_file = NULL;
    g_gnssState = GNSS_OPEN_STATE;

#ifdef CONFIG_LOWPOWER_GNSS_INIT
    // stop之后待机功耗才最低，所以执行一次快速的start、stop
    ret = uapi_gnss_start();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss start in init state fail, error code: %#x\n", ret);
        (void)gnss_deinit();
        return ret;
    }
    g_gnssState = GNSS_RUNNING_STATE;
    uapi_tcxo_delay_ms(GNSS_DELAY_TIME);

    ret = uapi_gnss_stop();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss stop in init state fail, error code: %#x\n", ret);
        (void)gnss_deinit();
        return ret;
    }
    uapi_tcxo_delay_ms(GNSS_DELAY_TIME);
#endif

    g_gnssState = GNSS_IDLE_STATE;

    return ret;
}

errcode_t gnss_start(void)
{
    if (g_gnssState != GNSS_IDLE_STATE) {
        GNSS_LOG_WARNING("[GNSS WARNING] gnss already start or not init, just return!\n");
        return ERRCODE_FAIL;
    }

    errcode_t ret = uapi_gnss_start();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss start fail, error code: %#x\n", ret);
        return ret;
    }
    g_gnssState = GNSS_RUNNING_STATE;
    return ret;
}

errcode_t gnss_stop(void)
{
    if (g_gnssState != GNSS_RUNNING_STATE) {
        GNSS_LOG_WARNING("[GNSS WARNING] gnss not start or not init, just return!\n");
        return ERRCODE_FAIL;
    }
    errcode_t ret = uapi_gnss_stop();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss stop fail, error code: %#x\n", ret);
        return ret;
    }
    g_gnssState = GNSS_IDLE_STATE;
    flash_gnss_log();
    return ret;
}

errcode_t gnss_deinit(void)
{
    if (g_gnssState == GNSS_NONE_STATE) {
        GNSS_LOG_WARNING("[GNSS WARNING] gnss already deinit or not init, just return!\n");
        return ERRCODE_FAIL;
    }
    errcode_t ret = ERRCODE_FAIL;
    if (g_gnssState == GNSS_RUNNING_STATE) {
        ret = gnss_stop();
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }
    g_gnssState = GNSS_IDLE_STATE;
    ret = uapi_gnss_close();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss close fail, error code: %#x\n", ret);
        return ret;
    }
    ret = uapi_gnss_deinit();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss deinit fail, error code: %#x\n", ret);
        return ret;
    }
    ret = uapi_gnss_power_off();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss power off fail, error code: %#x\n", ret);
        return ret;
    }
    g_gnssState = GNSS_NONE_STATE;
    flash_gnss_log();
    return ret;
}

errcode_t gnss_inject_time(GnssUtcTime *utcTime)
{
    uint8_t *buff = (uint8_t *)malloc(GNSS_ENCODE_BUFF_MAX_LEN);
    if (buff == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR]error: malloc failed in gnss_inject_time\n");
        return 0;
    }
    (void)memset_s(buff, GNSS_ENCODE_BUFF_MAX_LEN, 0, GNSS_ENCODE_BUFF_MAX_LEN);

    GnssMsg *hdr = (GnssMsg *)buff;
    hdr->cmd = CMD_XGNSS_INJECT_UTC_TIME;
    hdr->sequence = 0;
    get_sys_utc_time(utcTime);
    errno_t copyRet = memcpy_s(hdr->data, (GNSS_ENCODE_BUFF_MAX_LEN - sizeof(GnssMsg)), utcTime, sizeof(GnssUtcTime));
    if (copyRet != EOK) {
        GNSS_LOG_ERROR("[GNSS ERROR]memcpy UtcTime failed! Errno is %d\n", copyRet);
        free(buff);
        return ERRCODE_FAIL;
    }
    hdr->dataLength = sizeof(GnssUtcTime);
    hdr->checkSum = get_check_sum(hdr->data, hdr->dataLength);
    errcode_t ret = uapi_gnss_config((gnss_message_t *)buff);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR]gnss_inject_time error, error code: %#x\n", ret);
        free(buff);
        return ret;
    }

    return ret;
    free(buff);
}

errcode_t encode_and_inject_cmd(GnssCmdType cmdType, uint8_t* inBuff, uint32_t inLen)
{
    // 获取GNSS业务消息头
    int32_t len = 0;
    uint8_t *buff = (uint8_t *)malloc(GNSS_ENCODE_BUFF_MAX_LEN);
    if (buff == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR] malloc failed in encode_and_inject_cmd\n");
        return ERRCODE_FAIL;
    }
    memset_s(buff, GNSS_ENCODE_BUFF_MAX_LEN, 0, GNSS_ENCODE_BUFF_MAX_LEN);

    GnssMsg *gnssHdr = (GnssMsg *)buff;
    len += sizeof(GnssMsg);

    gnssHdr->sequence = 0;

    errno_t err = memcpy_s(buff + len, (GNSS_ENCODE_BUFF_MAX_LEN - len), inBuff, inLen);
    if (err != EOK) {
        GNSS_LOG_ERROR("[GNSS ERROR] Encode Cmd failed: %#x, err:%d\n", cmdType, err);
        free(buff);
        return ERRCODE_FAIL;
    }
    gnssHdr->cmd = cmdType;
    gnssHdr->dataLength = inLen;
    gnssHdr->checkSum = get_check_sum(gnssHdr->data, gnssHdr->dataLength);
    errcode_t ret = uapi_gnss_config((gnss_message_t *)gnssHdr);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR] inject cmd fail, error cmd:%#x code: %#x\n", gnssHdr->cmd, ret);
        free(buff);
        return ret;
    }

    GNSS_LOG_DEBUG("[GNSS DEBUG] inject cmd success: %#x\n", cmdType);
    free(buff);
    return ret;
}
