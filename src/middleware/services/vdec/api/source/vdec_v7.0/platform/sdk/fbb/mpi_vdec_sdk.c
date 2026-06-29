/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#include "mpi_vdec_sdk.h"
#include "securec.h"
#include "massdata_buffer.h"
#include "vdec_chr.h"

#define VDEC_KPI_TEST

STATIC td_void vdec_dfx_get_time_us(td_u32 *time_us)
{
    osal_timeval cur_time = { 0 };
    osal_gettimeofday(&cur_time);
    *time_us = (td_u32)(cur_time.tv_sec * 1000000 + cur_time.tv_usec); /* 1000000: index */
}

td_void vdec_get_time(td_u32 *time)
{
    td_u32 time_us;
    vdec_check(time != TD_NULL);

#ifdef VDEC_KPI_TEST
    vdec_dfx_get_time_us(&time_us);
    *time = time_us / 1000; /* 1000 num */
#endif

    return;
}

struct std_table_def {
    ext_vdec_standard std;
    td_char *protocol;
};

td_char* vdec_show_std(ext_vdec_standard std)
{
    td_u32 i;
    struct std_table_def std_to_string[] = {
        {EXT_VDEC_STD_H264,  "h264"},
        {EXT_VDEC_STD_MJPEG, "mjpeg"},
    };

    for (i = 0; i < (sizeof(std_to_string) / sizeof(std_to_string[0])); i++) {
        if (std_to_string[i].std == std) {
            return std_to_string[i].protocol;
        }
    }

    return "unkown";
}

STATIC td_s32 hexadecimal_number_required(td_u32 num)
{
    td_u32 tmp = num;
    td_s32 count = 0;

    while (tmp != 0) {
        tmp >>= 4; /* 4 is bits */
        count++;
    }
    return count;
}

td_void vdec_record_chr_event(td_s32 chan_id, vdec_chr_info_1 info_1, vdec_chr_info_2 info_2,
    vdec_chr_info_3 info_3)
{
    td_s32 ret;
    td_u8 info1_cvt;
    td_u8 info2_cvt;
    td_u32 info3_cvt;
    td_u32 tmp_num;
    td_u8 hex_width_info1;
    td_u8 hex_width_info2;
    td_u8 hex_width_info3;
    td_char tmp[10] = {0};

    if (info_1 >= MT_VDEC_INFO_1_MAX) {
        vdec_print_err("%s, %d, info_1 is beyond 0x%x 0x%x\r\n", __func__, __LINE__, MT_VDEC_INFO_1_MAX, info_1);
        return;
    }
    info1_cvt = (td_u8)info_1;
    hex_width_info1 = hexadecimal_number_required(info_1);

    if (info_2 >= MT_VDEC_INFO_2_MAX) {
        vdec_print_err("%s, %d, info_2 is beyond 0x%x 0x%x\r\n", __func__, __LINE__, MT_VDEC_INFO_2_MAX, info_2);
        return;
    }
    hex_width_info2 = hexadecimal_number_required(info_2);
    if (hex_width_info2 >= hex_width_info1) {
        tmp_num = info_1 << ((hex_width_info2 - hex_width_info1) * 4); /* 4 is time */
        info2_cvt = info_2 - tmp_num;
    } else {
        info2_cvt = 0;
    }

    if (info_3 == MT_VDEC_INFO_3_DEFAULT) {
        info3_cvt = 0;
    } else {
        hex_width_info3 = hexadecimal_number_required(info_3);
        tmp_num = info_2 << ((hex_width_info3 - hex_width_info2) * 4); /* 4 is time */
        info3_cvt = info_3 - tmp_num;
    }

    uapi_massdata_record_system_event(MT_VDEC_MANAGER, info1_cvt, info2_cvt, info3_cvt);
    if (chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM) {
        ret = sprintf_s(tmp, sizeof(tmp), "chan_id %d", chan_id);
        if (ret < 0) {
            vdec_print_err("%s, %d, err\r\n", __func__, __LINE__);
            return;
        }
        uapi_massdata_record_system_long_event(MT_VDEC_MANAGER, info1_cvt, info2_cvt, (td_u8 *)tmp, strlen(tmp));
    }
}
