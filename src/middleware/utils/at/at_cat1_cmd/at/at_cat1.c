/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: at cat1
 */


#include <string.h>
#include "watchdog.h"
#include "hal_reboot.h"
#include "at_cmd.h"
#include "at_cat1_cmd_register.h"
#include "at_cat1_cmd_table.h"
#ifdef SUPPORT_VOLTE_AUTOTEST
#include "ril_interface.h"
#endif
#include "imsa_interface.h"
#include "debug_print.h"
#include "oal_interface.h"
#if (CHIP_3322_V100 == 1)
#include "modem_service_msg.h"
#include "modem_service.h"
#endif
#include "at_cat1.h"

#define FWUPD_BUF_SIZE 256
#define CAT1_UPGRADE_FILE "/user/cat1_upgrade.bin"
#define BSSID_BYTE_2 2
#define BSSID_BYTE_3 3
#define BSSID_BYTE_4 4
#define BSSID_BYTE_5 5

#define AT_CAT1_FUNC_NUM (sizeof(at_cat1_cmd_parse_table) / sizeof(at_cat1_cmd_parse_table[0]))
#ifdef SUPPORT_IMS_CAT1_CHANNEL_TEST
#define CAT1_RST_DELAY  10000
#define CAT1_UTC_STR_LEN  50
#define AMR_TIME_SUM_CNT_MIN  100
#define AMR_TIME_SUM_CNT_MAX  3000
#endif

#ifdef SUPPORT_VOLTE_AUTOTEST
#define RIL_CMD_BACK_DATA_SIZE     512

void ZosGetTaskInfo(void);
uint32_t imsa_pip_spi_msg_send(uint8_t *data, uint32_t data_len);

int32_t ims_autotest_rsp_fun(uint8_t *data, size_t size)
{
    char rsp_str[RIL_CMD_BACK_DATA_SIZE] = {0};
    if (memcpy_s(rsp_str, RIL_CMD_BACK_DATA_SIZE, data, size) != EOK) {
        PRINT("ims autotest at rsp fail!\r\n");
        return AT_RET_MEM_API_ERROR;
    }
    PRINT("ims autotest at rsp: %s\r\n", rsp_str);
    return AT_RET_OK;
}

int sys_err_callback(uint8_t* data, size_t len)
{
    if (data == NULL || len == 0) {
        PRINT("sys err info len mistake\r\n");
        return AT_RET_CMD_PARA_ERROR;
    }
    PRINT("%s", (char*)data);

    return AT_RET_OK;
}
#endif

void los_at_cat1_cmd_register(void)
{
    PRINT("los_at_dcat1_cmd_register EXCUTE\r\n");
    uapi_at_cat1_register_cmd(at_cat1_cmd_parse_table, AT_CAT1_FUNC_NUM);
#ifdef SUPPORT_VOLTE_AUTOTEST
#if (CHIP_3322_V100 == 1)
    uapi_modem_service_register_handler(ims_autotest_rsp_fun);
#else
    ril_register_at_reply_callback(ims_autotest_rsp_fun);
#endif
    ril_register_sys_err_report_callback(sys_err_callback);
#endif
}

#ifdef SUPPORT_IMS_CAT1_CHANNEL_TEST
at_ret_t at_cat1_delay_reset(void)
{
    PRINT("enter at_cat1_delay_reset\n");
    oal_os_delay(CAT1_RST_DELAY);
    uapi_watchdog_disable();
    hal_reboot_chip();
    return AT_RET_OK;
}

at_ret_t at_cat1_chan_spi(const void *args)
{
    const cat1_chan_spi_args *spi_args = (const cat1_chan_spi_args *)args;
    PRINT("enter at_cat1_chan_spi:%d\n", spi_args->parm);
    uint8_t *send_data = (uint8_t *)malloc(spi_args->parm);
    if (send_data == NULL) {
        PRINT("malloc send data wrong\r\n");
        return AT_RET_MALLOC_ERROR;
    }

    uint8_t fill_num = 0;
    for (uint32_t i = 0; i < spi_args->parm; i++) {
        send_data[i] = fill_num++;
    }

    uint32_t ret = imsa_pip_spi_msg_send(send_data, spi_args->parm);
    free(send_data);
    if (ret != 0) {
        PRINT("spi_msg_send ret[%d]\r\n", ret);
        return AT_RET_RUN_ERROR;
    }
    return AT_RET_OK;
}

at_ret_t at_cat1_chan_loglevel(const void *args)
{
    const cat1_chan_loglevel_args *log_args = (const cat1_chan_loglevel_args *)args;
    PRINT("enter at_cat1_chan_loglevel:%d\n", log_args->parm);
    uint32_t ret = ril_set_user_log_level(log_args->parm);

    return ret == RIL_RET_SUCCESS ? AT_RET_OK : AT_RET_RUN_ERROR;
}

at_ret_t at_cat1_amr_to_spi_time(const void *args)
{
    const amr_to_spi_time_args *amr_args = (const amr_to_spi_time_args *)args;
    PRINT("enter amr_to_spi_time enable:%d\n", amr_args->parm);
    uint32_t *amr_time_sum_count = get_amr_time_sum_count();
 
    if (amr_args->parm) {
        bool *flag = get_amr_time_count_enable();
        *flag = true;
        if (amr_args->count < AMR_TIME_SUM_CNT_MIN) {
            *amr_time_sum_count = AMR_TIME_SUM_CNT_MIN;
        } else if (amr_args->count > AMR_TIME_SUM_CNT_MAX) {
            *amr_time_sum_count = AMR_TIME_SUM_CNT_MAX;
        } else {
            *amr_time_sum_count = amr_args->count;
        }
    } else {
        bool *flag = get_amr_time_count_enable();
        *flag = false;
    }
    return AT_RET_OK;
}

at_ret_t at_cat1_ril_utc(void)
{
    char utc_str[CAT1_UTC_STR_LEN] = {0};
    int32_t ret = ril_get_utc_time(utc_str, CAT1_UTC_STR_LEN);
    PRINT("utc_str:%s\r\n", utc_str);
    return ret == 0 ? AT_RET_OK : AT_RET_RUN_ERROR;
}

at_ret_t at_cat1_ril_sleep_time(const void *args)
{
    const ril_sleep_time_args *sleep_args = (const ril_sleep_time_args *)args;
    ril_set_fast_sleep_t fast_sleep = {0};
    fast_sleep.fast_sleep_switch = sleep_args->sleep_switch;
    fast_sleep.fast_sleep_time = sleep_args->sleep_time;

    int32_t ret = ril_set_fast_sleep_time(fast_sleep);
    return ret == 0 ? AT_RET_OK : AT_RET_RUN_ERROR;
}

at_ret_t at_cat1_ims_task_info(void)
{
    ZosGetTaskInfo();
    return AT_RET_OK;
}

at_ret_t at_cat1_debug(const void *args)
{
    const cat1_debug_args *debug_args = (const cat1_debug_args *)args;
    PRINT("parm : [%d][%d] \nstring :[%s] \n", debug_args->parm1, debug_args->parm2, debug_args->parm3);
    return AT_RET_OK;
}
#endif

at_ret_t at_cat1_ims_spi_test_handle(void)
{
    uint32_t ret = ims_factory_spi_test_handle();

    return ret == 0 ? AT_RET_OK : AT_RET_RUN_ERROR;
}

#ifdef SUPPORT_VOLTE_AUTOTEST
at_ret_t at_cat1_autotest(const void *args)
{
    const cat1_autotest_args *test_args = (const cat1_autotest_args *)args;
    if (test_args == NULL) {
        return AT_RET_CMD_PARA_ERROR;
    }

    PRINT("ims autotest cmd:%s\r\n", test_args->parm);
    char at_str[RIL_SEND_CMD_DATA_SIZE] = {0};
    if (snprintf_s(at_str, RIL_SEND_CMD_DATA_SIZE, RIL_SEND_CMD_DATA_SIZE - 1, "%s\r\n", test_args->parm) <= 0) {
        PRINT("ims autotest cmd invalid param!\r\n");
        return AT_RET_CHANNEL_PARA_ERROR;
    }

    // 异步执行at
    int32_t ret = ril_at_cmd_exc((char *)at_str);
    // 串口打印
    PRINT("ims autotest ret:%d\r\n", ret);
    return AT_RET_OK;
}
#endif

#ifdef SUPPORT_IMS_FUZZ_TEST
at_ret_t at_cat1_chan_fuzz(const void *args)
{
    const cat1_chan_fuzz_args *chan_args = (const cat1_chan_fuzz_args *)args;
    uint32_t ret = imsa_pip_chan_fuzz_fun_at_test((uint8_t)chan_args->parm);
    return ret == 0 ? AT_RET_OK : AT_RET_RUN_ERROR;
}
#endif

static uint8_t calc_crc8(const uint8_t *data, uint16_t len)
{
    uint8_t value = 0;
    while (len > 0) {
        value = value ^ (*(data++));
        len--;
    }
    return value;
}

static int32_t send_package_with_retry(ril_upadated_package_t *upg_pkg)
{
    int32_t ret = ril_systerm_updated_download_package(upg_pkg);
    if (ret == RIL_RET_FAIL_RECIVE_TIMEOUT || ret == RIL_RET_FAIL_RECIVE_ERR) {
        PRINT("ril download package sn=%u ret=%d, retry again.", upg_pkg->sn, ret);
        oal_os_delay(100); /* 100ms */
        ret = ril_systerm_updated_download_package(upg_pkg);
    }
    PRINT("ril download package sn=%u len=%u crc=%#x ret=%d\r\n", upg_pkg->sn, upg_pkg->len, upg_pkg->crc, ret);
    return ret;
}

at_ret_t cat1_upgrade_process(void)
{
    int32_t ret;
    int32_t upgrade_fd = open(CAT1_UPGRADE_FILE, O_RDONLY);
    if (upgrade_fd < 0) {
        PRINT("Failed to open %s\r\n", CAT1_UPGRADE_FILE);
        return ERRCODE_FAIL;
    }

    uint8_t *packet_buf = (uint8_t *)malloc(FWUPD_BUF_SIZE);
    if (packet_buf == NULL) {
        PRINT("ril systerm updated malloc fail\r\n");
        ret = ERRCODE_FAIL;
        goto cleanup;
    }

    ret = ril_systerm_updated_start();
    PRINT("ril systerm updated start ret=%d\r\n", ret);
    if (ret != RIL_RET_SUCCESS) {
        goto cleanup;
    }

    uint32_t sn = 0;
    ril_upadated_package_t upg_pkg;
    upg_pkg.download_type = 1;
    upg_pkg.data = packet_buf;

    while (1) {
        int32_t len = read(upgrade_fd, packet_buf, FWUPD_BUF_SIZE);
        if (len <= 0) {
            break;
        }

        if (len < FWUPD_BUF_SIZE) {
            memset_s(packet_buf + len, FWUPD_BUF_SIZE - len, 0, FWUPD_BUF_SIZE - len);
            len = FWUPD_BUF_SIZE;
        }

        upg_pkg.sn = sn++;
        upg_pkg.len = len;
        upg_pkg.crc = calc_crc8(packet_buf, len);
        ret = send_package_with_retry(&upg_pkg);
        if (ret != RIL_RET_SUCCESS) {
            break;
        }
    }

    ret = ril_systerm_updated_end();
    PRINT("ril systerm updated end ret=%d\r\n", ret);

cleanup:
    if (packet_buf != NULL) {
        free(packet_buf);
    }
    if (upgrade_fd >= 0) {
        close(upgrade_fd);
    }

    return ret;
}


at_ret_t at_cat1_ota_test(void)
{
    (void)cat1_upgrade_process();
    return ERRCODE_SUCC;
}
#if (CHIP_3322_V100 == 1)
// 状态字符串转换函数
const char* sim_status_to_string(sim_status_t status)
{
    switch (status) {
        case SIM_STATUS_REMOVED:
            return "SIM_STATUS_REMOVED";
        case SIM_STATUS_READY:
            return "SIM_STATUS_READY";
        case SIM_STATUS_PIN_BLOCKED:
            return "SIM_STATUS_PIN_BLOCKED";
        case SIM_STATUS_PUK_BLOCKED:
            return "SIM_STATUS_PUK_BLOCKED";
        default:
            return "UNKNOWN_STATUS";
    }
}

const char* network_reg_status_to_string(network_reg_status_t status)
{
    switch (status) {
        case REG_STATUS_NOT_REGISTERED:
            return "REG_STATUS_NOT_REGISTERED";
        case REG_STATUS_REGISTERED_HOME:
            return "REG_STATUS_REGISTERED_HOME";
        default:
            return "REG_STATUS_UNKNOWN";
    }
}

const char* flight_mode_status_to_string(flight_mode_status_t status)
{
    switch (status) {
        case FLIGHT_MODE_MINIMUM:
            return "FLIGHT_MODE_MINIMUM";
        case FLIGHT_MODE_FULL:
            return "FLIGHT_MODE_FULL";
        case FLIGHT_MODE_PSM:
            return "FLIGHT_MODE_PSM";
        default:
            return "FLIGHT_MODE_UNKNOWN";
    }
}

const char* ims_reg_status_to_string(ims_reg_status_t status)
{
    switch (status) {
        case IMS_REG_STATUS_NOT_REGISTERED:
            return "IMS_REG_STATUS_NOT_REGISTERED";
        case IMS_REG_STATUS_REGISTERED:
            return "IMS_REG_STATUS_REGISTERED";
        default:
            return "IMS_REG_STATUS_UNKNOWN";
    }
}

at_ret_t get_sim_card_status(void)
{
    sim_status_t status;
    int32_t ret = uapi_modem_get_sim_status(&status);
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to get SIM status\n");
        return AT_RET_RUN_ERROR;
    }
    PRINT("SIM Status: %s\n", sim_status_to_string(status));
    return AT_RET_OK;
}

at_ret_t get_network_reg_status(void)
{
    network_reg_status_t status;
    int32_t ret = uapi_modem_get_network_reg(&status);
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to get registration status\n");
        return AT_RET_RUN_ERROR;
    }
    PRINT("Network Registration Status: %s\n", network_reg_status_to_string(status));
    return AT_RET_OK;
}

at_ret_t get_operator_info_at(void)
{
    modem_operator_info operator_info;
    int32_t ret = uapi_modem_get_operator_info(&operator_info);
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to get operator info\n");
        return AT_RET_RUN_ERROR;
    }
    PRINT("Operator Info: MCC=%s, MNC=%s\n", operator_info.mcc, operator_info.mnc);
    return AT_RET_OK;
}

at_ret_t get_signal_info_at(void)
{
    soc_ue_cell_radio_info signal_info;
    int32_t ret = uapi_modem_get_signal_info(&signal_info);
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to get signal info\n");
        return AT_RET_RUN_ERROR;
    }
    PRINT("Signal Info: RSRP=%d, RSRQ=%d, RSSI=%d, SNR=%d\n",
          signal_info.rsrp, signal_info.rsrq, signal_info.rssi, signal_info.snr);
    return AT_RET_OK;
}

at_ret_t get_flight_mode_at(void)
{
    flight_mode_status_t status;
    int32_t ret = uapi_modem_get_flight_mode_status(&status);
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to get flight mode\n");
        return AT_RET_RUN_ERROR;
    }
    PRINT("Flight Mode Status: %s\n", flight_mode_status_to_string(status));
    return AT_RET_OK;
}

at_ret_t get_utc_time_at(void)
{
    time_t utc_time;
    int32_t tz_minutes;

    int32_t ret = uapi_modem_get_utc_time(&utc_time, &tz_minutes);
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to get UTC time\n");
        return AT_RET_RUN_ERROR;
    }

    struct timeval tv = { 0 };
    struct timezone tz = { 0 };
    tv.tv_sec = utc_time;
    tv.tv_usec = 0;
    tz.tz_dsttime = 0;
    tz.tz_minuteswest = tz_minutes;

    settimeofday(&tv, &tz);

    PRINT("Unix timestamp: %u tz_minutes: %d\n", utc_time, tz_minutes);
    return AT_RET_OK;
}

at_ret_t get_ims_reg_status_at(void)
{
    ims_reg_status_t status;
    int32_t ret = uapi_modem_get_ims_reg_status(&status);
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to get IMS registration status\n");
        return AT_RET_RUN_ERROR;
    }
    PRINT("IMS Registration Status: %s\n", ims_reg_status_to_string(status));
    return AT_RET_OK;
}

at_ret_t get_volte_status_at(void)
{
    call_state_t volte_status;
    int32_t ret = uapi_modem_get_volte_status(&volte_status);
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to get VoLTE status\n");
        return AT_RET_RUN_ERROR;
    }
    const char* state_str = NULL;
    switch (volte_status) {
        case CALL_STATE_ACTIVE:
            state_str = "CALL_STATE_ACTIVE";
            break;
        case CALL_STATE_CALL_ON_HOLD:
            state_str = "CALL_STATE_CALL_ON_HOLD";
            break;
        case CALL_STATE_OUTGOING_CALL:
            state_str = "CALL_STATE_OUTGOING_CALL";
            break;
        case CALL_STATE_RINGTONE:
            state_str = "CALL_STATE_RINGTONE";
            break;
        case CALL_STATE_INCOMING_CALL:
            state_str = "CALL_STATE_INCOMING_CALL";
            break;
        default:
            state_str = "CALL_STATE_INACTIVE";
            break;
    }
    PRINT("volte Status: %s (value: %d)\n", state_str, volte_status);
    return AT_RET_OK;
}

at_ret_t get_cat1_network_status_at(void)
{
    cat1_network_reg_status_t status;
    int32_t ret = uapi_modem_get_cat1_network_status(&status);
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to get cat1 network status\n");
        return AT_RET_RUN_ERROR;
    }
    const char* status_str = NULL;
    switch (status) {
        case CAT1_REG_STATUS_NOT_REGISTERED:
            status_str = "CAT1_REG_STATUS_NOT_REGISTERED";
            break;
        case CAT1_REG_STATUS_REGISTERED_HOME:
            status_str = "CAT1_REG_STATUS_REGISTERED_HOME";
            break;
        case CAT1_REG_STATUS_SEARCHING:
            status_str = "CAT1_REG_STATUS_SEARCHING";
            break;
        case CAT1_REG_STATUS_DENIED:
            status_str = "CAT1_REG_STATUS_DENIED";
            break;
        case CAT1_REG_STATUS_REGISTERED_ROAMING:
            status_str = "CAT1_REG_STATUS_REGISTERED_ROAMING";
            break;
        case CAT1_REG_STATUS_EMERGENCY_ONLY:
            status_str = "CAT1_REG_STATUS_EMERGENCY_ONLY";
            break;
        default:
            status_str = "CAT1_REG_STATUS_UNKNOWN";
            break;
    }

    PRINT("CAT1 Network Status: %s (value: %d)\n", status_str, status);
    return AT_RET_OK;
}

int32_t get_modem_rate_report_callback(uint8_t* data, size_t len)
{
    if (data == NULL || len < sizeof(modem_net_rate_info_t)) {
        return ERRCODE_INVALID_PARAM;
    }
    modem_net_rate_info_t* rate_info = (modem_net_rate_info_t*)data;
    PRINT("modem rate %u %u \n", rate_info->cur_rate, rate_info->next_rate);
    return ERRCODE_SUCC;
}

at_ret_t get_rate_report_start(void)
{
    PRINT("get_rate_report_start\n");
    int32_t ret = uapi_modem_rate_report_start(1, get_modem_rate_report_callback);
    if (ret != AT_RET_OK) {
        PRINT("Failed to start modem rate report\n");
        return AT_RET_RUN_ERROR;
    }
    return AT_RET_OK;
}

at_ret_t get_rate_report_stop(void)
{
    PRINT("get_rate_report_stop\n");
    int32_t ret = uapi_modem_rate_report_stop();
    if (ret != AT_RET_OK) {
        PRINT("Failed to stop modem rate report\n");
        return AT_RET_RUN_ERROR;
    }
    return AT_RET_OK;
}

int32_t get_modem_wifi_info_callback(uint8_t* data, size_t len)
{
    if (data == NULL || len < sizeof(modem_wifi_scan_result_t)) {
        return ERRCODE_INVALID_PARAM;
    }
    modem_wifi_scan_result_t* wifi_info = (modem_wifi_scan_result_t*)data;
    PRINT("Wi-Fi: ssid=%.32s\r\n", wifi_info->ssid);
    PRINT("Wi-Fi: rssi=%d\r\n", wifi_info->rssi);
    PRINT("Wi-Fi: bssid=%02x:%02x:%02x:%02x:%02x:%02x\r\n",
          wifi_info->bssid[0], wifi_info->bssid[1], wifi_info->bssid[BSSID_BYTE_2],
          wifi_info->bssid[BSSID_BYTE_3], wifi_info->bssid[BSSID_BYTE_4], wifi_info->bssid[BSSID_BYTE_5]);
    return ERRCODE_SUCC;
}

at_ret_t get_wifi_info(void)
{
    int32_t ret = uapi_modem_wifi_scan(get_modem_wifi_info_callback);
    if (ret != AT_RET_OK) {
        PRINT("Failed to get wifiinfo\n");
        return AT_RET_RUN_ERROR;
    }
    return AT_RET_OK;
}
#endif