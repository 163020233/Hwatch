/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide api functions for at \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-21， Create file. \n
 */
#include <string.h>
#include <stdlib.h>
#include "stdint.h"
#include "at.h"
#include "app_at_uart.h"
#include "ux_at_manager.h"
#include "ux_api_fwk.h"
#include "ux_cmd_audio.h"
#include "ux_common_log.h"
#include "ux_at_interface.h"
#include "ux_device_manager.h"
#include "ux_at_system_test.h"
#include "ux_system_manager.h"
#include "ux_api_communication.h"
#include "ux_at_logic_id_test.h"
#include "ux_sync_commu.h"
#include "test/ux_at_power_test.h"
#include "ux_cmd_utils.h"
#ifndef DEMO_WATCH
#include "feature_config.h"
#endif
#ifdef CONFIG_SUPPORT_MUSIC_ATTRIBUTES
#include "ux_hal_memory.h"
#include "bs_api_stru.h"
#endif
#include "test/ux_at_usb_test.h"
#include "test/ux_at_display_test.h"
#include "test/ux_at_spp_test.h"
#include "test/ux_at_ble_test.h"
#include "ux_at_audio_test.h"
#include "test/ux_at_test.h"

errcode_t ux_at_test_char_to_num_16(uint8_t ch, uint8_t *num)
{
    if (ch >= '0' && ch <= '9') {
        *num = ch - '0';
        return ERRCODE_SUCC;
    } else if (ch >= 'a' && ch <= 'f') {
        *num = ch - 'a' + 10;    /* a stand for 10 in hex */
        return ERRCODE_SUCC;
    } else if (ch >= 'A' && ch <= 'F') {
        *num = ch - 'A' + 10;    /* A stand for 10 in hex */
        return ERRCODE_SUCC;
    }
    return ERRCODE_FAIL;
}

errcode_t ux_at_test_fixed_str_to_data(uint16_t str_len, uint8_t *str, uint16_t data_len, uint8_t *data)
{
    uint8_t *str_cpy = str;
    uint16_t len = str_len;
    uint8_t num_h;
    uint8_t num_l;

    if (len != data_len * 2) {    /* the length of data is 2 times that of str or plus 2 for "0x" */
        if (len != data_len * 2 + 2) {    /* the length of data is 2 times that of str or plus 2 for "0x" */
            return ERRCODE_FAIL;
        }
        if (!(str_cpy[0] == '0' && (str_cpy[1] == 'x' || str_cpy[1] == 'X'))) {
            return ERRCODE_FAIL;
        }
        str_cpy += 2;    /* 2 0x or 0X length */
        len -= 2;    /* 2 0x or 0X length */
    }
    for (uint16_t i = 0; i < len - 1; i += 2) {    /* 2 Array subscript change value */
        if ((ux_at_test_char_to_num_16(str_cpy[i], &num_h) == ERRCODE_FAIL) ||
            (ux_at_test_char_to_num_16(str_cpy[i + 1], &num_l) == ERRCODE_FAIL)) {
            return ERRCODE_FAIL;
        }
        /* the length of data is 2 times that of str, 4 is high bits for bytes */
        data[i / 2] = (uint8_t)((num_h << 4) | num_l);
    }
    return ERRCODE_SUCC;
}

errcode_t ux_at_test_str_to_data(const char *str, uint16_t data_len, uint8_t *data)
{
    return ux_at_test_fixed_str_to_data((uint16_t)strlen((char*)str), (uint8_t *)str, data_len, data);
}

errcode_t ux_at_test_fill_data(void *dst, size_t dst_len, void *src, size_t src_len)
{
    if (dst == NULL || src == NULL || src_len == 0) {
        return ERRCODE_FAIL;
    }

    size_t len = 0;
    if (memmove_s((char *)dst + len, dst_len - len, src, MIN(src_len, dst_len - len)) != EOK) {
        return ERRCODE_FAIL;
    }

    for (; len < dst_len; len += src_len) {
        if (memmove_s((char *)dst + len, dst_len - len, dst, MIN(len, dst_len - len)) != EOK) {
            return ERRCODE_FAIL;
        }
    }
    return ERRCODE_SUCC;
}

uint32_t ux_at_test_get_digit(const char *param)
{
    char *p = NULL;
    char *str = strtok_r((char *)param, "\r", &p);
    if (str == NULL) {
        return 0;
    }

    uint32_t number = (uint32_t)strtol(str, NULL, 0);

    return number;
}

#ifdef CONFIG_SUPPORT_PEER_COMMU_TEST
static errcode_t notify_send_msg_test(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    at_uart_printf("notify_send_msg_test receive message:\n");
    at_uart_print((char *)data, data_len);
    ux_log_debug("notify_send_msg_test end\n");
    return ERRCODE_SUCC;
}

#ifdef CONFIG_SUPPORT_MUSIC_ATTRIBUTES
static errcode_t process_music_element_attr_msg(uint8_t sub_event, uint8_t *data, uint16_t data_len)
{
    at_uart_printf(" ========= music info ========= \r\n");
    if (data == NULL || data_len == 0) {
        at_uart_printf("\r\nreceived music_element_attr_msg invalid\r\n");
        return ERRCODE_INVALID_PARAM;
    }
    if (sub_event <= 0 || sub_event > BS_MEDIA_ELEMENT_ATTR_TRACK_PLAYING_TIME) {
        return ERRCODE_INVALID_PARAM;
    }
    at_uart_printf("\r\nattrId-valLen: 0x%x-%u \r\n", sub_event, data_len);
    switch (sub_event) {
        case BS_MEDIA_ELEMENT_ATTR_TITLE:
            at_uart_printf("Attr Title:\r\n");
            break;
        case BS_MEDIA_ELEMENT_ATTR_ARTIST_NAME:
            at_uart_printf("Attr Name:\r\n");
            break;
        case BS_MEDIA_ELEMENT_ATTR_ALBUM_NAME:
            at_uart_printf("Attr Album:\r\n");
            break;
        case BS_MEDIA_ELEMENT_ATTR_TRACK_NUMBER:
            at_uart_printf("Attr Track Number:\r\n");
            break;
        case BS_MEDIA_ELEMENT_ATTR_TOTAL_NUMBER_OF_TRACKS:
            at_uart_printf("Attr Total Number Of Track:\r\n");
            break;
        case BS_MEDIA_ELEMENT_ATTR_TRACK_GENRE:
            at_uart_printf("Attr Track GENRE:\r\n");
            break;
        case BS_MEDIA_ELEMENT_ATTR_TRACK_PLAYING_TIME:
            at_uart_printf("Attr Track Playing Time_ms:\r\n");
            break;
        default:
            break;
    }
    uint8_t *value = (uint8_t *)ux_hal_memory_acquire(UX_MEM_BT, data_len + 1);
    if (value == NULL) {
        at_uart_printf("Attr Malloc fail \r\n");
        return ERRCODE_MALLOC;
    }
    if (data_len > 0 && data != NULL) {
        (void)memcpy_s(value, data_len, data, data_len);
    }
    value[data_len] = '\0';
    at_uart_printf(" %s \r\n", value);
    ux_hal_memory_release(UX_MEM_BT, value);
    value = NULL;
    return UX_ERR_SUCCESS;
}
#endif
#endif

static errcode_t commu_open_send_msg_test(const char *device,
    const char *param, const char *sub_param)
{
    unused(device);
    unused(param);
    unused(sub_param);
    errcode_t commu_ret = ERRCODE_SUCC;
    errcode_t attr_ret = ERRCODE_SUCC;
#ifdef CONFIG_SUPPORT_PEER_COMMU_TEST
    commu_ret = ux_cmd_utils_commu_register_msg_handler(UX_API_COMMU_CMD_NOTIFY_SEND_MSG_TEST, notify_send_msg_test);
#ifdef CONFIG_SUPPORT_MUSIC_ATTRIBUTES
    attr_ret = ux_api_fwk_module_register_prio(UX_EVENT_TYPE_COMMU, process_music_element_attr_msg, UX_API_PRIO_HIGH);
#endif
#endif
    return (commu_ret == ERRCODE_SUCC) && (attr_ret == ERRCODE_SUCC) ? ERRCODE_SUCC : ERRCODE_FAIL;
}

static errcode_t commu_send_msg(const char *device,
    const char *param, const char *sub_param)
{
    unused(device);
    unused(param);
    unused(sub_param);

#ifdef CONFIG_SUPPORT_PEER_COMMU_TEST
    if (device == NULL || param == NULL) {
        ux_log_err("[ux][UXTEST]ux_at_test_commu_set_process invalid param\n");
        return ERRCODE_FAIL;
    }
    uint16_t dst_dev_id = (uint8_t)atoi(device);
    const char *data = param;
    uint16_t data_len = (uint16_t)strlen(data);

    return ux_cmd_utils_commu_send_msg_to_device(dst_dev_id, (uint8_t *)data, data_len);
#endif
    return ERRCODE_FAIL;
}

/**
 * AT指令格式: AT^UXTEST=commu,open_send_msg_test 打开通路测试
 * AT指令格式: AT^UXTEST=commu,send_msg,dev_id,data 发送数据
 * 举例: AT^UXTEST=commu,send_msg,1,1111
 * dev_id: 设备ID,保留字段
 * data: 数据内容, 最大支持30字节
 */
static errcode_t ux_at_test_commu_set_process(const char *func_name, const char *device,
    const char *param, const char *sub_param)
{
    if (strcasecmp (func_name, "open_send_msg_test") == 0) {      // AT^UXTEST=commu,open_send_msg_test
        return commu_open_send_msg_test(device, param, sub_param);
    } else if (strcasecmp (func_name, "send_msg") == 0) {         // AT^UXTEST=commu,send_msg,dev_id,data
        return commu_send_msg(device, param, sub_param);
    }
    return ERRCODE_FAIL;
}

static errcode_t ux_at_test_query_cmd(const char *module_name, const char *func_name, const char *device,
    const char *param, const char *sub_param)
{
    ux_at_test_cmd_map_t func_map[] = {
        { "system", ux_at_test_system_query_process },
        { "audio", ux_at_test_audio_query_process },
#if defined(MELODY_DEMO_BMIC_RX)
        { "device", ux_at_test_logic_id_query_process },
#endif
#if defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_BMIC_TX)
        { "power", ux_at_test_power_query_process },
#endif
#ifdef CONFIG_SUPPORT_JACK
        { "jack", ux_at_test_jack_query_process },
#ifdef CONFIG_SUPPORT_SOURCE_MANAGER_FOCUS_MODE
        { "play_mode", ux_at_test_play_mode_query_process },
#endif
#endif
#ifndef DEMO_WATCH
        { "sle", ux_at_test_sle_query_process },
#endif
    };

    uint32_t cmd_cnt = (uint32_t)arry_num(func_map);
    for (uint32_t i = 0; i < cmd_cnt; i++) {
        if (strcasecmp(module_name, func_map[i].cmd) == 0) {
            return func_map[i].func(func_name, device, param, sub_param);
        }
    }

    return ERRCODE_FAIL;
}

static errcode_t ux_at_test_set_cmd(const char *module_name, const char *func_name, const char *device,
    const char *param, const char *sub_param)
{
    ux_at_test_cmd_map_t func_map[] = {
#if defined(MELODY_DEMO_BMIC_RX)
        { "display", ux_at_test_display_set_process },
#endif
#if defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_BMIC_TX)
        { "power", ux_at_test_power_set_process },
#endif
#if defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_SOUNDBAR) || defined(MELODY_DEMO_BMIC_RX)
        { "usb", ux_at_test_usb_set_process },
#endif
#ifdef CONFIG_SUPPORT_CAPSENSOR
        { "sensor", ux_at_test_sensor_set_process },
#endif
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA) || defined(DEMO_WATCH)
        { "spp", ux_at_test_spp_set_process },
        { "ble", ux_at_test_ble_set_process },
#endif
#ifndef DEMO_WATCH
        { "system", ux_at_test_system_set_process },
        { "audio", ux_at_test_audio_set_process },
        { "sle", ux_at_test_sle_set_process },
#endif
        { "commu", ux_at_test_commu_set_process },
    };

    uint32_t cmd_cnt = (uint32_t)arry_num(func_map);
    for (uint32_t i = 0; i < cmd_cnt; i++) {
        if (strcasecmp(module_name, func_map[i].cmd) == 0) {
            return func_map[i].func(func_name, device, param, sub_param);
        }
    }

    return ERRCODE_FAIL;
}

at_ret_t at_ux_test_query_process(const ux_at_para5_args_t *args)
{
    errcode_t ret = ERRCODE_SUCC;
    const char *module_name = NULL;
    const char *func_name = NULL;

    if (args == NULL) {
        at_uart_printf("[ux][UXTEST]args is null\n");
        return AT_RET_SYNTAX_ERROR;
    }

    module_name = args->para1;
    if (module_name == NULL) {
        at_uart_printf("[ux][UXTEST]module_name is null\n");
        return AT_RET_SYNTAX_ERROR;
    }

    func_name = args->para2;
    if (func_name == NULL) {
        at_uart_printf("[ux][UXTEST]function_name is null\n");
        return AT_RET_SYNTAX_ERROR;
    }

    ret = ux_at_test_query_cmd(module_name, func_name, args->para3, args->para4, args->para5);
    if (ret == ERRCODE_SUCC) {
        return AT_RET_OK;
    }
    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_ux_test_set_process(const ux_at_para5_args_t *args)
{
    errcode_t ret = ERRCODE_SUCC;
    const char *module_name = NULL;
    const char *func_name = NULL;

    if (args == NULL) {
        at_uart_printf("[ux][UXTEST]args is null\n");
        return AT_RET_SYNTAX_ERROR;
    }

    module_name = args->para1;
    if (module_name == NULL) {
        at_uart_printf("[ux][UXTEST]module_name is null\n");
        return AT_RET_SYNTAX_ERROR;
    }

    func_name = args->para2;
    if (func_name == NULL) {
        at_uart_printf("[ux][UXTEST]function_name is null\n");
        return AT_RET_SYNTAX_ERROR;
    }

    ret = ux_at_test_set_cmd(module_name, func_name, args->para3, args->para4, args->para5);
    if (ret == ERRCODE_SUCC) {
        return AT_RET_OK;
    }

    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_ux_test_read_process(void)
{
    at_uart_printf("\r\n^UXTEST READ OK\r\n");
    return AT_RET_OK;
}
