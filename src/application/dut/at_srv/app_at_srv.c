/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2024. All rights reserved.
 * Description:  AT service entrance.
 */

#include <common_def.h>
#include <securec.h>
#include <platform_core.h>
#include <soc_osal.h>
#include <at_product.h>
#include <systick.h>
#include <app_at_init.h>
#include <app_at_uart.h>
#include <diag_log.h>
#if defined(CONFIG_AT_SUPPORT_RESET_VETO)
#include <pm_veto.h>
#endif
#include <app_at_srv.h>

void app_at_ack(const char *ack, uint16_t length)
{
    at_uart_print(ack, length);
}

#ifdef CONFIG_SUPPORT_AT_UART
STATIC void at_process(const uint8_t *data, uint32_t length)
{
    char *buf;
    char* head = NULL;
    char* next_head = NULL;
    char* context = NULL;
    uint32_t len = length;

#if defined(CONFIG_AT_SUPPORT_RESET_VETO)
    /* 收到AT命令后延时5000ms再允许进入深睡眠 */
    uapi_pm_add_sleep_veto_with_timeout(PM_ID_DEBUG, 5000);
#endif

    if ((data == NULL) || (length == 0)) {
        app_at_ack("AT^FAIL\r\n", strlen("AT^FAIL\r\n"));
        return;
    }

    /* Add \0 to make sure data is str */
    buf = malloc(length + 1);
    if (buf == NULL) {
        return;
    }
    if (memcpy_s(buf, length + 1, data, length) != EOK) {
        free(buf);
        return;
    }

    buf[length] = '\0';

    head = strtok_s(buf, "\n", &context);
    while (head) {
        next_head = strtok_s(NULL, "\n", &context);
        if (next_head == NULL) {
            /* Send the last AT */
            errcode_t ret = uapi_at_channel_data_recv(AT_UART_PORT, (uint8_t*)head, len);
            if (ret != ERRCODE_SUCC) {
                uapi_diag_error_log(0, "AT channel answer error 0x%x\n", ret);
            }
            free(buf);
            return;
        }

        if ((uint32_t)(next_head - head) < len) {
            /* Has more then one AT command */
            errcode_t ret = uapi_at_channel_data_recv(AT_UART_PORT, (uint8_t*)head, next_head - head);
            if (ret != ERRCODE_SUCC) {
                uapi_diag_error_log(0, "AT channel answer error 0x%x\n", ret);
            }

            osal_udelay(50); /* Delay 50ms for AT channel busy */
            len = len - (next_head - head);
            head = next_head;
        }
    }
    free(buf);
}
#endif

errcode_t app_at_srv_init(void)
{
    app_at_init();
#ifdef CONFIG_SUPPORT_AT_UART
    app_at_uart_init(AT_UART_BUS);
    app_at_uart_data_rx_reg(at_process);
#endif
    return ERRCODE_SUCC;
}

