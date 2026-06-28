/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides AT report source \n
 *
 * History: \n
 * 2022-09-02， Create file. \n
 */

#include "at_product.h"
#include "at_channel.h"
#include "at_process.h"
#include "at_base.h"


#if defined CONFIG_AT_PRINT_BUFFER_SIZE
#define AT_PRINT_BUFFER_SIZE CONFIG_AT_PRINT_BUFFER_SIZE
#else
#define AT_PRINT_BUFFER_SIZE 128
#endif

void uapi_at_report(const char* str)
{
    if (str == NULL) {
        return;
    }

    uint16_t channel_id = at_proc_get_current_channel_id();
    at_write_func_t func = at_channel_get_write_func(channel_id);
    if (func != NULL) {
        func((char*)str);
    }

    at_log_normal(str, strlen(str), 0);
}

void uapi_at_print(const char* str, ...)
{
    int32_t len;
    va_list args;
    static char str_buf[AT_PRINT_BUFFER_SIZE] = {0};
    char *tmp_buf = NULL;
    uint32_t buflen = AT_PRINT_BUFFER_SIZE;

    if (str == NULL) {
        return;
    }

    va_start(args, str);
    tmp_buf = str_buf;
    len = vsnprintf_s(tmp_buf, buflen, buflen - 1, str, args);
    va_end(args);
    if ((len == -1) || (*tmp_buf == '\0')) {
        /* parameter is illegal or some features in fmt dont support */
        return;
    }

    uapi_at_report(tmp_buf);
}

void uapi_at_report_to_single_channel(at_channel_id_t channel_id, const char* str)
{
    if (str == NULL) {
        return;
    }

    at_write_func_t func = at_channel_get_write_func(channel_id);
    if (func != NULL) {
        func((char*)str);
    }

    at_log_normal(str, strlen(str), 0);
}