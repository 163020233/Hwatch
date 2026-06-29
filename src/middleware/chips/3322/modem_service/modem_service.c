/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: modem_service
 * Create: 2026
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "errcode.h"
#include "ril_interface.h"
#include "debug_print.h"
#include "modem_service_event.h"
#include "modem_service.h"

#define MAX_RESPONSE_BUFFER_SIZE 128
#define UTC_LEN 64
#define MINUTES_PER_QUARTER 15
#define VOLTE_STATUS_LEN 16
#define NETWORK_STATUS_STR_LEN 16
#define UTC_DATA_TIME_STR_LEN 32
#define INDEX_0 0
#define INDEX_1 1
#define INDEX_2 2
#define INDEX_3 3
#define INDEX_4 4
#define INDEX_5 5
#define CCLK_PREFIX_LENGTH 8 // "+CCLK: \""长度
#define WIFI_LEN 64
#define NWIFISCAN_PREFIX_LENGTH 11 // "+NWIFISCAN:"长度
#define RATE_BUFFER_SIZE 16
#define HEX_PAIR_LENGTH 2
#define BASE_DECIMAL 10
#define AT_COMMAND_SIZE 32

event_callback_t g_event_callbacks[MODEM_EVENT_MAX] = { 0 };

typedef struct {
    const char* command_str;
    int32_t (*callback)(uint8_t* data, size_t len);
} command_callback_map_t;

int32_t modem_net_rate_process(uint8_t* data, size_t len);
int32_t modem_wifi_scan_process(uint8_t* data, size_t len);

static const command_callback_map_t command_map[] = {
    {"+NRATEADAPTULTHP", modem_net_rate_process},
    {"+NWIFISCAN", modem_wifi_scan_process},
};

static char* skip_whitespace(char* str)
{
    while (*str == ' ' || *str == '\t') {
        str++;
    }
    return str;
}

static char* find_field_end(char* start, char* end)
{
    char* final_end = end;
    if (end != NULL) {
        while (final_end > start &&
               (*final_end == ' ' || *final_end == '\t' ||
                *final_end == '\r' || *final_end == '\n')) {
            final_end--;
        }
        if (final_end < start) {
            final_end = start;
        }
        if (final_end > start) {
            final_end++;
        } else if (final_end == start) {
            final_end = end;
        }
    } else {
        final_end = start + strlen(start);
        while (final_end > start &&
               (*(final_end - 1) == ' ' || *(final_end - 1) == '\t' ||
                *(final_end - 1) == '\r' || *(final_end - 1) == '\n')) {
            final_end--;
        }
    }
    return final_end;
}

int32_t parse_at_response_field(const char *response, const char *field_name,
                                int field_index, char *output, size_t output_size)
{
    if (response == NULL || field_name == NULL || output == NULL || output_size == 0) {
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    char *start_index = strstr(response, field_name);
    if (start_index == NULL) {
        return RIL_RET_FAIL_PROCESS;
    }

    start_index += strlen(field_name);
    start_index = skip_whitespace(start_index);

    // 查找字段
    for (int i = 0; i < field_index; i++) {
        start_index = strchr(start_index, ',');
        if (start_index == NULL) {
            return RIL_RET_FAIL_PROCESS;
        }
        start_index++; // 跳过逗号
        start_index = skip_whitespace(start_index);
    }

    // 查找字段结束位置
    char *end_index = strchr(start_index, ',');
    char *final_end_index = find_field_end(start_index, end_index);

    // 计算字段长度
    size_t field_len = final_end_index - start_index;
    if (field_len >= output_size) {
        field_len = output_size - 1;
    }

    errno_t err = strncpy_s(output, output_size, start_index, field_len);
    if (err != 0) {
        return RIL_RET_FAIL_PROCESS;
    }

    return RIL_RET_SUCCESS;
}

int32_t uapi_modem_get_sim_status(sim_status_t *status)
{
    int32_t ret;
    bool is_usim_in_position = false;
    bool is_usim_pin_block = false;
    bool is_usim_puk_block = false;

    if (status == NULL) {
        PRINT("Invalid parameter: status is NULL\r\n");
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    ret = ril_get_is_usim_in_position(&is_usim_in_position);
    if (ret != RIL_RET_SUCCESS) {
        return ret;
    }

    ret = ril_get_is_usim_pin_block(&is_usim_pin_block);
    if (ret != RIL_RET_SUCCESS) {
        return ret;
    }

    ret = ril_get_is_usim_puk_block(&is_usim_puk_block);
    if (ret != RIL_RET_SUCCESS) {
        return ret;
    }

    if (!is_usim_in_position) {
        *status = SIM_STATUS_REMOVED;
    } else if (is_usim_puk_block) {
        *status = SIM_STATUS_PUK_BLOCKED;
    } else if (is_usim_pin_block) {
        *status = SIM_STATUS_PIN_BLOCKED;
    } else {
        *status = SIM_STATUS_READY;
    }

    return RIL_RET_SUCCESS;
}

int32_t uapi_modem_get_network_reg(network_reg_status_t *status)
{
    int32_t ret;
    soc_ue_cell_info reg_status;

    if (status == NULL) {
        PRINT("Invalid parameter: status is NULL\r\n");
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    ret = ril_get_ue_registration_status(&reg_status);
    if (ret != RIL_RET_SUCCESS) {
        return ret;
    }

    if (!reg_status.valid) {
        *status = REG_STATUS_NOT_REGISTERED;
    } else if (reg_status.primary_cell) {
        *status = REG_STATUS_REGISTERED_HOME;
    } else {
        *status = REG_STATUS_UNKNOWN;
    }

    return RIL_RET_SUCCESS;
}

int32_t uapi_modem_get_operator_info(modem_operator_info *operator_info)
{
    int32_t ret;
    char at_cmd[] = "AT+NUESTATS=SERVINGCELL\r\n";
    char *response_buffer = NULL;

    if (operator_info == NULL) {
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    response_buffer = (char *)malloc(MAX_RESPONSE_BUFFER_SIZE);
    if (response_buffer == NULL) {
        PRINT("Failed to allocate memory for response buffer\r\n");
        return RIL_RET_FAIL_PROCESS;
    }
    ret = ril_at_cmd_sync_get_info(at_cmd, (uint8_t *)response_buffer, MAX_RESPONSE_BUFFER_SIZE, AT_CMD_TIMEOUT);
    if (ret != RIL_RET_SUCCESS) {
        free(response_buffer);
        return ret;
    }

    // 解析MCC字段（索引4）
    ret = parse_at_response_field(response_buffer, "+NUESTATS:",
                                  INDEX_4, operator_info->mcc, sizeof(operator_info->mcc));
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to parse MCC field, error: %d\r\n", ret);
        free(response_buffer);
        return ret;
    }

    // 解析MNC字段（索引5）
    ret = parse_at_response_field(response_buffer, "+NUESTATS:",
                                  INDEX_5, operator_info->mnc, sizeof(operator_info->mnc));
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to parse MNC field, error: %d\r\n", ret);
        free(response_buffer);
        return ret;
    }

    free(response_buffer);
    return RIL_RET_SUCCESS;
}

int32_t uapi_modem_get_signal_info(soc_ue_cell_radio_info *signal_info)
{
    int32_t ret;

    if (signal_info == NULL) {
        PRINT("Invalid parameter: signal_info is NULL\r\n");
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    ret = ril_get_radio_signal_info(signal_info);
    if (ret != RIL_RET_SUCCESS) {
        return ret;
    }

    return RIL_RET_SUCCESS;
}

int32_t uapi_modem_get_flight_mode_status(flight_mode_status_t *status)
{
    int32_t ret;
    char *response_buffer = NULL;

    if (status == NULL) {
        PRINT("Invalid parameter: status is NULL\r\n");
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    response_buffer = (char *)malloc(MAX_RESPONSE_BUFFER_SIZE);
    if (response_buffer == NULL) {
        PRINT("Failed to allocate memory for response buffer\r\n");
        return RIL_RET_FAIL_PROCESS;
    }
    ret = ril_at_cmd_sync_get_info("AT+CFUN?\r\n", (uint8_t *)response_buffer,
                                   MAX_RESPONSE_BUFFER_SIZE, AT_CMD_TIMEOUT);
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to get flight mode status, error: %d\r\n", ret);
        free(response_buffer);
        return ret;
    }

    if (strstr(response_buffer, "+CFUN: 0") != NULL) {
        *status = FLIGHT_MODE_MINIMUM;
    } else if (strstr(response_buffer, "+CFUN: 1") != NULL) {
        *status = FLIGHT_MODE_FULL;
    } else if (strstr(response_buffer, "+CFUN: 4") != NULL) {
        *status = FLIGHT_MODE_PSM;
    } else {
        *status = FLIGHT_MODE_UNKNOWN;
    }

    free(response_buffer);
    return RIL_RET_SUCCESS;
}

static int32_t parse_time_string(const char* date_time_str, time_t* timestamp, int32_t *tz_offset)
{
    int32_t year, month, day, hour, minute, second, tz;

    int result = sscanf_s(date_time_str, "%2d/%2d/%2d,%2d:%2d:%2d%3d",
                          &year, &month, &day, &hour, &minute, &second, &tz);
    if (result != 7) { /* 7个字段 */
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    *tz_offset =  tz * MINUTES_PER_QUARTER;

    struct tm tm_info = {0};
    tm_info.tm_year = year + 100; /* 1900 + 100 */
    tm_info.tm_mon = month - 1;
    tm_info.tm_mday = day;
    tm_info.tm_hour = hour;
    tm_info.tm_min = minute;
    tm_info.tm_sec = second;

    time_t rawtime = timegm(&tm_info);
    if (rawtime == (time_t)-1) {
        return RIL_RET_FAIL_INVALID_PARAM;
    }
    *timestamp = rawtime;

    return RIL_RET_SUCCESS;
}

static int32_t extract_time_string(const char* time_buffer, char* date_time_str, size_t str_size)
{
    char *time_start = strstr(time_buffer, "+CCLK: \"");
    if (time_start == NULL) {
        PRINT("Failed to find time string start\r\n");
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    if (strlen(time_start) < CCLK_PREFIX_LENGTH) {
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    time_start += CCLK_PREFIX_LENGTH;

    char *time_end = strchr(time_start, '"');
    if (time_end == NULL) {
        PRINT("Failed to find time string end\r\n");
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    size_t time_len = time_end - time_start;
    if (time_len >= str_size) {
        time_len = str_size - 1;
    }

    errno_t err = strncpy_s(date_time_str, str_size, time_start, time_len);
    if (err != EOK) {
        PRINT("Failed to copy date_time string, error code: %d\r\n", err);
        return RIL_RET_FAIL_PROCESS;
    }
    date_time_str[time_len] = '\0';

    return RIL_RET_SUCCESS;
}

int32_t uapi_modem_get_utc_time(time_t* utc_time, int32_t *tz_minutes)
{
    int32_t ret;
    char time_buffer[UTC_LEN] = {0};
    char date_time_str[UTC_DATA_TIME_STR_LEN] = {0};
    time_t timestamp = 0;
    int32_t tz_offset = 0;

    if (utc_time == NULL || tz_minutes == NULL) {
        PRINT("Invalid parameter: utc_time or tz_minutes is NULL\r\n");
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    ret = ril_get_utc_time(time_buffer, UTC_LEN);
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to get UTC time, error: %d\r\n", ret);
        return ret;
    }

    ret = extract_time_string(time_buffer, date_time_str, sizeof(date_time_str));
    if (ret != RIL_RET_SUCCESS) {
        return ret;
    }

    PRINT("Extracted date_time string: %s\r\n", date_time_str);

    ret = parse_time_string(date_time_str, &timestamp, &tz_offset);
    if (ret != RIL_RET_SUCCESS) {
        return ret;
    }

    *utc_time = timestamp;
    *tz_minutes = tz_offset;

    return RIL_RET_SUCCESS;
}

int32_t uapi_modem_get_ims_reg_status(ims_reg_status_t *status)
{
    int32_t ret;
    bool is_ims_reg = false;

    if (status == NULL) {
        PRINT("Invalid parameter: status is NULL\r\n");
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    ret = ril_get_radio_ims_reg(&is_ims_reg);
    if (ret != RIL_RET_SUCCESS) {
        return ret;
    }

    if (!is_ims_reg) {
        *status = IMS_REG_STATUS_NOT_REGISTERED;
    } else {
        *status = IMS_REG_STATUS_REGISTERED;
    }

    return RIL_RET_SUCCESS;
}

static int32_t parse_volte_status(const char* state_str, call_state_t* status)
{
    char *endptr;
    long temp;
    if (state_str == NULL || status == NULL) {
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    if (strlen(state_str) == 0) {
        return RIL_RET_FAIL_PROCESS;
    }

    temp = strtol(state_str, &endptr, BASE_DECIMAL);
    if (endptr == state_str) {
        return RIL_RET_FAIL_PROCESS;
    }
    int parsed_state = (int)temp;
    switch (parsed_state) {
        case 0: // 0：激活状态
            *status = CALL_STATE_ACTIVE;
            break;
        case 1: // 1：呼叫保持状态
            *status = CALL_STATE_CALL_ON_HOLD;
            break;
        case 2: // 2：主叫，拨号状态
            *status = CALL_STATE_OUTGOING_CALL;
            break;
        case 3: // 3：主叫，回铃音状态
            *status = CALL_STATE_RINGTONE;
            break;
        case 4: // 4：被叫，来电状态
            *status = CALL_STATE_INCOMING_CALL;
            break;
        default: // 未激活状态
            *status = CALL_STATE_INACTIVE;
            break;
    }

    return RIL_RET_SUCCESS;
}

int32_t uapi_modem_get_volte_status(call_state_t* status)
{
    int32_t ret;
    char *response_buffer = NULL;
    char state_str[VOLTE_STATUS_LEN];

    if (status == NULL) {
        PRINT("Invalid parameter: status is NULL\n");
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    response_buffer = (char *)malloc(MAX_RESPONSE_BUFFER_SIZE);
    if (response_buffer == NULL) {
        PRINT("Failed to allocate memory for response buffer\n");
        return RIL_RET_FAIL_PROCESS;
    }

    ret = ril_at_cmd_sync_get_info("AT+CLCC\r\n", (uint8_t *)response_buffer, MAX_RESPONSE_BUFFER_SIZE, AT_CMD_TIMEOUT);
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to get VoLTE status, error: %d\n", ret);
        free(response_buffer);
        return ret;
    }

    if (strstr(response_buffer, "+CLCC: 0") != NULL) {
        *status = CALL_STATE_INACTIVE;
        PRINT("CALL_STATE_INACTIVE\n");
        free(response_buffer);
        return RIL_RET_SUCCESS;
    }

    // 解析state字段（索引2）
    ret = parse_at_response_field(response_buffer, "+CLCC:", INDEX_2, state_str, sizeof(state_str));
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to parse state field, error: %d\n", ret);
        free(response_buffer);
        return ret;
    }

    ret = parse_volte_status(state_str, status);
    free(response_buffer);
    return ret;
}

static int32_t parse_network_status(const char* status_str, cat1_network_reg_status_t* status)
{
    char *endptr;
    long temp;
    if (status_str == NULL || status == NULL) {
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    if (strlen(status_str) == 0) {
        PRINT("Invalid status string: empty string\r\n");
        return RIL_RET_FAIL_PROCESS;
    }

    temp = strtol(status_str, &endptr, BASE_DECIMAL);
    if (endptr == status_str) {
        return RIL_RET_FAIL_PROCESS;
    }
    int parsed_status = (int)temp;
    switch (parsed_status) {
        case 0: // 0：未注册，MT当前未搜索要注册的网络
            *status = CAT1_REG_STATUS_NOT_REGISTERED;
            break;
        case 1: // 1：注册，家庭网络
            *status = CAT1_REG_STATUS_REGISTERED_HOME;
            break;
        case 2: // 2：未注册，但MT当前正在尝试附着或搜索要注册的网络
            *status = CAT1_REG_STATUS_SEARCHING;
            break;
        case 3: // 3：注册拒绝
            *status = CAT1_REG_STATUS_DENIED;
            break;
        case 5: // 5：注册，漫游
            *status = CAT1_REG_STATUS_REGISTERED_ROAMING;
            break;
        case 8: // 8：仅用于紧急承载业务
            *status = CAT1_REG_STATUS_EMERGENCY_ONLY;
            break;
        default: // 未知
            *status = CAT1_REG_STATUS_UNKNOWN;
            break;
    }

    return RIL_RET_SUCCESS;
}

int32_t uapi_modem_get_cat1_network_status(cat1_network_reg_status_t *status)
{
    int32_t ret;
    char *response_buffer = NULL;
    char status_str[NETWORK_STATUS_STR_LEN];

    if (status == NULL) {
        PRINT("Invalid parameter: status is NULL\r\n");
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    response_buffer = (char *)malloc(MAX_RESPONSE_BUFFER_SIZE);
    if (response_buffer == NULL) {
        PRINT("Failed to allocate memory for response buffer\r\n");
        return RIL_RET_FAIL_PROCESS;
    }

    ret = ril_at_cmd_sync_get_info("AT+CEREG?\r\n", (uint8_t *)response_buffer,
                                   MAX_RESPONSE_BUFFER_SIZE, AT_CMD_TIMEOUT);
    if (ret != RIL_RET_SUCCESS) {
        free(response_buffer);
        return ret;
    }

    // 解析状态字段（索引1）
    ret = parse_at_response_field(response_buffer, "+CEREG:", 1, status_str, sizeof(status_str));
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to parse status field, error: %d\r\n", ret);
        free(response_buffer);
        return ret;
    }

    ret = parse_network_status(status_str, status);
    free(response_buffer);
    return ret;
}

int32_t modem_event_register_handler(modem_event_type_t event_type, event_callback_t callback)
{
    if (callback == NULL || event_type >= MODEM_EVENT_MAX) {
        return ERRCODE_INVALID_PARAM;
    }

    g_event_callbacks[event_type] = callback;

    return ERRCODE_SUCC;
}

int32_t modem_event_unregister_handler(modem_event_type_t event_type)
{
    if (event_type >= MODEM_EVENT_MAX) {
        return ERRCODE_INVALID_PARAM;
    }

    g_event_callbacks[event_type] = NULL;

    return ERRCODE_SUCC;
}

int32_t modem_message_process(uint8_t* data, size_t len)
{
    if (data == NULL || len == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    char* cmd_str = (char*)data;
    for (size_t i = 0; i < sizeof(command_map) / sizeof(command_map[0]); i++) {
        size_t cmd_len =  strlen(command_map[i].command_str);
        if ((cmd_len <= len) && (strstr(cmd_str, command_map[i].command_str) != NULL)) {
            return command_map[i].callback(data, len);
        }
    }

    return ERRCODE_SUCC;
}

int32_t uapi_modem_rate_report_start(uint32_t period, event_callback_t callback)
{
    int32_t ret;
    char at_command[AT_COMMAND_SIZE] = {0};
    uint8_t response_buffer[AT_COMMAND_SIZE] = {0};
 
    if (snprintf_s(at_command, sizeof(at_command), sizeof(at_command), "AT+NRATEADAPTPARA=%u,,,\r\n", period) < 0) {
        return RIL_RET_FAIL_PROCESS;
    }
 
    ret = ril_at_cmd_sync_get_info(at_command, response_buffer, AT_COMMAND_SIZE, AT_CMD_TIMEOUT);
    if (ret != RIL_RET_SUCCESS) {
        return ret;
    }
    (void)response_buffer;
 
    modem_event_register_handler(MODEM_EVENT_NET_RATE, callback);
 
    if (snprintf_s(at_command, sizeof(at_command), sizeof(at_command), "AT+NRATEADAPT=%u\r\n", period) < 0) {
        return RIL_RET_FAIL_PROCESS;
    }
 
    ret = ril_at_cmd_exc(at_command);
    if (ret != RIL_RET_SUCCESS) {
        return ret;
    }
 
    return ERRCODE_SUCC;
}

int32_t uapi_modem_rate_report_stop(void)
{
    int32_t ret;
    modem_event_unregister_handler(MODEM_EVENT_NET_RATE);
    ret = ril_at_cmd_exc("AT+NRATEADAPT=0\r\n");
    if (ret != RIL_RET_SUCCESS) {
        return ret;
    }
    return ERRCODE_SUCC;
}

int32_t modem_net_rate_process(uint8_t* data, size_t len)
{
    int32_t ret;
    uint32_t cur_rate, next_rate;
    char *endptr;
    long temp;
    char cur_rate_buffer[RATE_BUFFER_SIZE];
    char next_rate_buffer[RATE_BUFFER_SIZE];
    modem_net_rate_info_t rate_info = { 0 };
    if (data == NULL || len == 0) {
        return ERRCODE_INVALID_PARAM;
    }
    ret = parse_at_response_field((char *)data, "+NRATEADAPTULTHP:",
                                  INDEX_0, cur_rate_buffer, sizeof(cur_rate_buffer));
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to parse first number, error: %d\r\n", ret);
        return ret;
    }
    ret = parse_at_response_field((char *)data, "+NRATEADAPTULTHP:",
                                  INDEX_1, next_rate_buffer, sizeof(next_rate_buffer));
    if (ret != RIL_RET_SUCCESS) {
        PRINT("Failed to parse second number, error: %d\r\n", ret);
        return ret;
    }

    temp = strtol(cur_rate_buffer, &endptr, BASE_DECIMAL);
    cur_rate = (uint32_t)temp;
    temp = strtol(next_rate_buffer, &endptr, BASE_DECIMAL);
    next_rate = (uint32_t)temp;
    rate_info.cur_rate = cur_rate;
    rate_info.next_rate = next_rate;
    event_callback_t callback = g_event_callbacks[MODEM_EVENT_NET_RATE];
    if (callback != NULL) {
        callback((uint8_t*)&rate_info, sizeof(rate_info));
    }

    return ERRCODE_SUCC;
}

int32_t uapi_modem_wifi_scan(event_callback_t callback)
{
    int32_t ret;
    modem_event_register_handler(MODEM_EVENT_WIFI_INFO, callback);
    ret = ril_at_cmd_exc("AT+NWIFISCAN\r\n");
    if (ret != RIL_RET_SUCCESS) {
        return ret;
    }
    return ERRCODE_SUCC;
}

static int process_wifi_line(char *line_start, modem_wifi_scan_result_t *wifi_result)
{
    int32_t ret;
    char field_buffer[WIFI_LEN];
    char *endptr;
    long temp;

    if (line_start == NULL || wifi_result == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    // 解析SSID字段（索引1）
    ret = parse_at_response_field(line_start, "+NWIFISCAN:", 1,
                                  (char*)wifi_result->ssid, sizeof(wifi_result->ssid));
    if (ret != RIL_RET_SUCCESS) {
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    if (wifi_result->ssid[0] == '\0') {
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    // 解析RSSI字段（索引3）
    ret = parse_at_response_field(line_start, "+NWIFISCAN:", INDEX_3,
                                  field_buffer, sizeof(field_buffer));
    if (ret != RIL_RET_SUCCESS) {
        return RIL_RET_FAIL_INVALID_PARAM;
    }
    temp = strtol(field_buffer, &endptr, BASE_DECIMAL);
    wifi_result->rssi = (int8_t)temp;
    
    // 解析BSSID字段（索引4）
    char bssid[WIFI_ADDR_LEN * HEX_PAIR_LENGTH + 1] = { 0 };
    ret = parse_at_response_field(line_start, "+NWIFISCAN:", INDEX_4,
                                  bssid, sizeof(bssid));
    if (ret != RIL_RET_SUCCESS) {
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    int32_t bssid_num[WIFI_ADDR_LEN] = { 0 };
    int result = sscanf_s(bssid, "%2x%2x%2x%2x%2x%2x", &bssid_num[0], &bssid_num[1], &bssid_num[2],
                                        &bssid_num[3], &bssid_num[4], &bssid_num[5]);
    if (result != WIFI_ADDR_LEN) {
        return RIL_RET_FAIL_INVALID_PARAM;
    }

    for (int i = 0; i < WIFI_ADDR_LEN; i++) {
        wifi_result->bssid[i] = (uint8_t)bssid_num[i];
    }

    // 解析channel_num字段（索引5）
    ret = parse_at_response_field(line_start, "+NWIFISCAN:", INDEX_5, field_buffer, sizeof(field_buffer));
    if (ret != RIL_RET_SUCCESS) {
        return RIL_RET_FAIL_INVALID_PARAM;
    }
    temp = strtol(field_buffer, &endptr, BASE_DECIMAL);
    wifi_result->channel_num = (uint8_t)temp;

    return ERRCODE_SUCC;
}

int32_t modem_wifi_scan_process(uint8_t* data, size_t len)
{
    int32_t ret;
    char *line_start = NULL;
    char *line_end = NULL;
    modem_wifi_scan_result_t wifi_result;

    if (data == NULL || len == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    line_start = (char*)data;

    while (*line_start == '\n' || *line_start == '\r') {
        line_start++;
    }

    line_end = strchr(line_start, '\n');
    if (line_end != NULL) {
        *line_end = '\0';
    }

    if (strncmp(line_start, "+NWIFISCAN:", NWIFISCAN_PREFIX_LENGTH) == 0) {
        if (strstr(line_start, "+NWIFISCAN: 0") == line_start) {
            return ERRCODE_SUCC;
        }

        errno_t err = memset_s(&wifi_result, sizeof(wifi_result), 0, sizeof(wifi_result));
        if (err != EOK) {
            return ERRCODE_FAIL;
        }

        ret = process_wifi_line(line_start, &wifi_result);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }

        event_callback_t callback = g_event_callbacks[MODEM_EVENT_WIFI_INFO];
        if (callback != NULL) {
            callback((uint8_t*)&wifi_result, sizeof(wifi_result));
        }
    }

    return ERRCODE_SUCC;
}