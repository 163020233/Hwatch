/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: at adapter \n
 * Author: @CompanyNameTag \n
 * 2024-01-10, Create file. \n
 */
#include <stdint.h>
#include <stdbool.h>
#include "soc_osal.h"
#include "common_def.h"
#include "ipc.h"
#include "at_product.h"
#include "at_config.h"
#include "debug_print.h"
#include "securec.h"
#include "uart.h"
#include "soc_errno.h"
#include "chip_io.h"
#include "at_cmd.h"
#ifdef CFG_DRIVERS_USB_AS_SERIAL
#include "usb_porting.h"
#endif
#include "uart_porting.h"
#ifdef AT_COMMAND_VDEC
#include "at_vdec_cmd_register.h"
#endif
#ifdef AT_COMMAND_VENC
#include "at_venc_cmd_register.h"
#endif
#ifdef AT_COMMAND_ISP
#include "at_isp_cmd_register.h"
#endif
#ifdef AT_COMMAND_CAMERA
#include "at_camera_cmd_register.h"
#endif
#include "at_adapter.h"
#ifdef AT_COMMAND_GPU
#include "at_gpu.h"
#include "at_gpu_cmd_register.h"
#endif
#ifdef AT_COMMAND_AUDIO
#include "at_audio_cmd_register.h"
#endif
#ifdef AT_COMMAND_GNSS
#include "at_gnss_cmd_register.h"
#endif
#ifdef AT_COMMAND_DITING_GNSS
#include "at_diting_gnss_cmd_register.h"
#endif
#ifdef AT_COMMAND_WEAR
#include "at_wear_cmd_register.h"
#endif
#ifdef AT_COMMAND_NPU
#include "at_npu_cmd_register.h"
#endif
#ifdef AT_COMMAND_GUI
#include "at_gui_cmd_register.h"
#endif
#ifdef AT_COMMAND_MEDIA
#include "at_media_cmd_register.h"
#endif
#ifdef AT_COMMAND_OHOS
#include "at_ohos_cmd_register.h"
#endif
#ifdef AT_DITING_EXAMPLE_GPIO
#include "gpio.h"
#endif
#ifdef AT_DITING_EXAMPLE_PWM
#include "pwm_demo.h"
#endif
#ifdef AT_DITING_EXAMPLE_TIMER
#include "timer_demo.h"
#endif
#ifdef AT_DITING_EXAMPLE_WATCHDOG
#include "watchdog_demo.h"
#endif
#ifdef AT_COMMAND_NET
#include "at_net_cmd_register.h"
#endif
#ifdef AT_COMMAND_NFC
#include "at_nfc_cmd_register.h"
#endif
#ifdef AT_COMMAND_CAT1
#include "at_cat1_cmd_register.h"
#endif
#include "non_os_reboot.h"
#ifdef FT_DONGLE
#include "diag_service.h"
#endif
#ifdef SUPPORT_ESIM
#include "at_esim_cmd_register.h"
#endif

#define AT_LOG_MAX_LEN 512

#ifdef FT_DONGLE
#define FT_DONGLE_PAYLOAD_POS 3
#endif

#if defined(CONFIG_BG_SLE_ONLY) && (CONFIG_BG_SLE_ONLY == 1)
void bth_sle_common_at_cmd_register(void);
#elif defined(HI3322_WSTP) && (HI3322_WSTP == 1)
void bth_sle_common_at_cmd_register(void);
void bth_br_common_at_cmd_register(void);
void bth_ble_common_at_cmd_register(void);
void bs_at_common_register(void);
errcode_t bt_ft_atcmd_init(void);
errcode_t bt_cali_at_cmd_init(void);
#endif

#if defined(SUPPORT_BGH_L3_AT) && (SUPPORT_BGH_L3_AT == 1)
void sle_service_at_init(void);
#endif

#if defined(CHBA_SUPPORT)
void sle_at_chba_register(void);
#endif

void at_sle_data_trans_cmd_register(void);

static void at_write_data(const uint8_t *data, uint32_t data_len)
{
    if (data == NULL) {
        return;
    }
#ifdef CFG_DRIVERS_USB_AS_SERIAL
    uapi_usb_serial_write(USB_SERIAL_0, data, data_len);
#else
    uapi_uart_write(AT_UART_BUS, data, data_len, 0);
#endif
}

static void at_write_func(const char *data)
{
    at_write_data((const uint8_t *)data, strlen(data));
}

static void at_base_api_queue_create(uint32_t msg_count, uint32_t msg_size, unsigned long *queue_id)
{
    osal_msg_queue_create(NULL, (uint16_t)msg_count, queue_id, 0, (uint16_t)msg_size);
}

static uint32_t at_base_api_msg_queue_write(unsigned long queue_id, void *msg_ptr, uint32_t msg_size, uint32_t timeout)
{
    return (uint32_t)osal_msg_queue_write_copy(queue_id, msg_ptr, msg_size, timeout);
}

static uint32_t at_base_api_msg_queue_read(unsigned long queue_id, void *buf_ptr, uint32_t *buf_size, uint32_t timeout)
{
    return (uint32_t)osal_msg_queue_read_copy(queue_id, buf_ptr, buf_size, timeout);
}

static void at_base_api_task_pause(void)
{
    osal_yield();
}

static void* at_base_api_malloc(uint32_t size)
{
    return osal_kmalloc(size, OSAL_GFP_ATOMIC);
}

static void at_base_api_free(void *addr)
{
    osal_kfree(addr);
}

static void at_base_api_register(void)
{
    at_base_api_t base_api = {
        .msg_queue_create_func = at_base_api_queue_create,
        .msg_queue_write_func = at_base_api_msg_queue_write,
        .msg_queue_read_func = at_base_api_msg_queue_read,
        .task_pause_func = at_base_api_task_pause,
        .malloc_func = at_base_api_malloc,
        .free_func = at_base_api_free,
    };
    uapi_at_base_api_register(base_api);
}


void at_adapter_print(const char *fmt, ...)
{
    static char log[AT_LOG_MAX_LEN] = {0};
    int32_t str_len;

    if (fmt == NULL) {
        return;
    }
    va_list args;
    va_start(args, fmt);
    str_len = vsprintf_s(log, AT_LOG_MAX_LEN, fmt, args);
    va_end(args);

    if (str_len < 0) {
        return;
    }

    at_write_func(log);
}

#define AT_BASE_FUNC_NUM (sizeof(at_base_register_parse_table) / sizeof(at_base_register_parse_table[0]))
#define EXT_AT_BASE_CMD_MAX_LEN 128

at_ret_t at_base_rst(void)
{
    uapi_at_report("reset system...\r\n");
    reboot_chip();
    return 0;
}

at_ret_t at_help(void)
{
    uint32_t i;
    uint32_t cnt = at_cmd_get_entry_total();
    at_cmd_entry_t *cmd_entry = NULL;
    at_cmd_entry_t **cmd_tbl = (at_cmd_entry_t **)malloc(sizeof(at_cmd_entry_t *) * cnt);

    if (cmd_tbl == NULL) {
        return AT_RET_MALLOC_ERROR;
    }
    if (at_cmd_get_all_entrys((const at_cmd_entry_t **)cmd_tbl, cnt) != cnt) {
        free(cmd_tbl);
        return AT_RET_MEM_API_ERROR;
    }

    for (i = 0; i < cnt; ++i) {
        cmd_entry = (at_cmd_entry_t *)cmd_tbl[i];
        at_adapter_print("NAME:%s\r\n", cmd_entry->name);
    }
    free(cmd_tbl);
    return AT_RET_OK;
}

const at_cmd_entry_t at_base_register_parse_table[] = {
    {
        "RST",
        1,
        0,
        NULL,
        at_base_rst,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "HELP",
        4,
        0,
        NULL,
        at_help,
        NULL,
        NULL,
        NULL,
    },
};

void at_base_cmd_register(void)
{
    uint32_t ret = EXT_ERR_FAILURE;

    ret = uapi_at_cmd_table_register(at_base_register_parse_table,
                                     (uint32_t)AT_BASE_FUNC_NUM, EXT_AT_BASE_CMD_MAX_LEN);
    if (ret != 0) {
        uapi_at_report_to_single_channel(0, "uapi_at_register_cmd \r\n");
    }
}

#define CRLF_STR                      "\r\n"
#define CR_ASIC_II                    0xD

void at_channel_rx_callback(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    if (length == 0) {
        PRINT("at uart receive err len=0\r\n");
        return;
    }

    if (((char *)buffer)[0] == CR_ASIC_II) {
        at_write_data((const uint8_t *)CRLF_STR, (uint16_t)strlen(CRLF_STR));
    } else {
        at_write_data((const uint8_t *)buffer, (uint32_t)length);
    }

#ifdef AT_DEBUG
    uint32_t ret = uapi_at_channel_data_recv(AT_UART_PORT, (uint8_t *)buffer, (uint32_t)length);
    if (ret != ERRCODE_SUCC) {
        PRINT("at callback exec fail [%x]\r\n", ret);
    }
#else
    uapi_at_channel_data_recv(AT_UART_PORT, (uint8_t *)buffer, (uint32_t)length);
#endif
}

static uint8_t g_at_test_uart_rx_buffer_test[AT_RX_BUFFER_SIZE];

static void at_uart_init(void)
{
#ifndef FT_SINGLE_UART
    uapi_uart_deinit(AT_UART_BUS);
    uart_buffer_config_t uart_buffer_config;
    uart_pin_config_t uart_pin_config = {
        .tx_pin = AT_UART_TX_PIN,
        .rx_pin = AT_UART_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };
    uart_attr_t uart_line_config = {
        .baud_rate = AT_UART_BAUD_RATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };
    uart_buffer_config.rx_buffer_size = sizeof(g_at_test_uart_rx_buffer_test);
    uart_buffer_config.rx_buffer = g_at_test_uart_rx_buffer_test;
    errcode_t ret = uapi_uart_init(AT_UART_BUS, &uart_pin_config, &uart_line_config, NULL, &uart_buffer_config);
    if (ret != 0) {
        PRINT("at uart init error ret=%d\r\n", ret);
    }
    uapi_uart_register_rx_callback(AT_UART_BUS, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
        AT_RX_CONDITION_SIZE, at_channel_rx_callback);
#endif
}

static void at_cmd_init_part(void)
{
#ifdef AT_COMMAND_VDEC
    at_vdec_cmd_register();
#endif
#ifdef AT_COMMAND_VENC
    at_venc_cmd_register();
#endif
#ifdef AT_COMMAND_ISP
    at_isp_cmd_register();
#endif
#ifdef AT_COMMAND_CAMERA
    at_camera_cmd_register();
#endif
}

#ifdef FT_DONGLE
STATIC errcode_t mcu_diag_cmd_process(diag_ser_data_t *data)
{
    uint16_t length = data->header.length;
    uint8_t *buf = data->payload + FT_DONGLE_PAYLOAD_POS + 1;
    at_channel_rx_callback(buf, length, 0);
    return ERRCODE_SUCC;
}

void mcu_diag_cmd_int(void)
{
    uapi_diag_service_register(DIAG_SER_APP_TEST, mcu_diag_cmd_process);
}
#endif

void at_adapter_init(void)
{
#ifndef CFG_DRIVERS_USB_AS_SERIAL
    at_uart_init();
#endif
    at_base_api_register();
    uapi_at_channel_write_register(AT_UART_PORT, at_write_func);
    at_base_cmd_register();
#if defined(CONFIG_BG_SLE_ONLY) && (CONFIG_BG_SLE_ONLY == 1)
    bth_sle_common_at_cmd_register();
#elif defined(HI3322_WSTP) && (HI3322_WSTP == 1)
    bth_sle_common_at_cmd_register();
    bth_br_common_at_cmd_register();
    bth_ble_common_at_cmd_register();
    bt_ft_atcmd_init();
    bt_cali_at_cmd_init();
#if defined(SUPPORT_BGH_L3_AT) && (SUPPORT_BGH_L3_AT == 1)
    bs_at_common_register();
    sle_service_at_init();
#endif
#if defined(CHBA_SUPPORT)
    sle_at_chba_register();
#endif
    at_sle_data_trans_cmd_register();       /* 数传sample AT命令注册 */
#endif
    at_cmd_init_part();
#ifdef AT_COMMAND_GPU
    at_gpu_cmd_register();
#endif
#ifdef AT_COMMAND_WEAR
    at_wear_cmd_register();
#endif
#ifdef AT_COMMAND_NPU
    at_npu_cmd_register();
#endif
#ifdef AT_COMMAND_AUDIO
    at_audio_cmd_register();
#endif
#ifdef AT_COMMAND_GNSS
    at_gnss_cmd_register();
#endif
#ifdef AT_COMMAND_DITING_GNSS
    at_diting_gnss_cmd_register();
#endif
#ifdef AT_COMMAND_GUI
    at_gui_cmd_register();
#endif
#ifdef AT_COMMAND_MEDIA
    at_media_cmd_register();
#endif
#ifdef AT_COMMAND_OHOS
    at_ohos_cmd_register();
#endif
#ifdef AT_COMMAND_NET
    at_net_cmd_register();
#endif
#ifdef AT_COMMAND_NFC
    at_nfc_cmd_register();
#endif
#ifdef AT_COMMAND_CAT1
    at_cat1_cmd_register();
#endif
#ifdef FT_DONGLE
    mcu_diag_cmd_int();
#endif
#ifdef SUPPORT_ESIM
    at_esim_cmd_register();
#endif
#ifdef AT_DITING_EXAMPLE_GPIO
    at_diting_gpio_example_cmd_register();
#endif
#ifdef AT_DITING_EXAMPLE_PWM
    at_diting_pwm_example_cmd_register();
#endif
#ifdef AT_DITING_EXAMPLE_TIMER
at_diting_timer_example_cmd_register();
#endif
#ifdef AT_DITING_EXAMPLE_WATCHDOG
    at_diting_watchdog_example_cmd_register();
#endif
}
