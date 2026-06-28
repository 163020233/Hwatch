/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: jpeg parse
 */

#include "jpeg_parse.h"
#include "securec.h"
#include "jpeg_stream.h"
#include "jpeg_table.h"
#include "jpeg_exif.h"

/*-------------------------------------- macro define -------------------------------------*/
#define MARK_TAG   (JPEG_MARK_TAG)
#define MARK_SOI   (JPEG_MARK_SOI)
#define MARK_EOI   (JPEG_MARK_EOI)
/* tem mark */
#define MARK_TEM   0x01
/* sofn mark */
#define MARK_SOF0  0xc0
#define MARK_SOF1  0xc1
#define MARK_SOF2  0xc2
#define MARK_SOF3  0xc3
#define MARK_SOF5  0xc5
#define MARK_SOF6  0xc6
#define MARK_SOF7  0xc7
#define MARK_JPG   0xc8
#define MARK_SOF9  0xc9
#define MARK_SOF10 0xca
#define MARK_SOF11 0xcb
#define MARK_SOF13 0xcd
#define MARK_SOF14 0xce
#define MARK_SOF15 0xcf
/* dht mark */
#define MARK_DHT   0xc4
/* dac mark */
#define MARK_DAC   0xcc
/* rst mark */
#define MARK_RST0  0xd0
#define MARK_RST1  0xd1
#define MARK_RST2  0xd2
#define MARK_RST3  0xd3
#define MARK_RST4  0xd4
#define MARK_RST5  0xd5
#define MARK_RST6  0xd6
#define MARK_RST7  0xd7
/* compress data */
#define MARK_SOS   0xda
/* dqt mark */
#define MARK_DQT   0xdb
/* dnl mark */
#define MARK_DNL   0xdc
/* dri mark */
#define MARK_DRI   0xdd
/* appn mark */
#define MARK_APP0  0xe0
#define MARK_APP1  0xe1
#define MARK_APP2  0xe2
#define MARK_APP3  0xe3
#define MARK_APP4  0xe4
#define MARK_APP5  0xe5
#define MARK_APP6  0xe6
#define MARK_APP7  0xe7
#define MARK_APP8  0xe8
#define MARK_APP9  0xe9
#define MARK_APP10 0xea
#define MARK_APP11 0xeb
#define MARK_APP12 0xec
#define MARK_APP13 0xed
#define MARK_APP14 0xee
#define MARK_APP15 0xef
/* com mark */
#define MARK_COM   0xfe

#define JPEG_DATA_PRECISION  8
#define JPEG_YUV_COMPONENTS  3
#define JPEG_MAX_SAMP_FACTOR 4

#ifdef GPU_1A22
#define JPEG_MIN_WIDTH       8
#define JPEG_MIN_HEIGHT      8
#define JPEG_MAX_WIDTH       4096
#define JPEG_MAX_HEIGHT      4096
#else
#define JPEG_MIN_WIDTH       10
#define JPEG_MIN_HEIGHT      10
#define JPEG_MAX_WIDTH       8096
#define JPEG_MAX_HEIGHT      8096
#endif

STATIC td_s32 jpeg_max(td_s32 a, td_s32 b)
{
    return (a < b) ? b : a;
}

/*-------------------------------------- struct info --------------------------------------*/
typedef enum {
    JPEG_PARSE_NOT_JPEG = 0,
    JPEG_PARSE_SOFN,
    JPEG_PARSE_SOS,
    JPEG_PARSE_DQT,
    JPEG_PARSE_DHT,
    JPEG_PARSE_DRI,
    JPEG_PARSE_CONTINUE,
    JPEG_PARSE_ERROR,
    JPEG_PARSE_FINISH,
    JPEG_PARSE_MAX
} jpeg_parse_state;

/*-------------------------------------- func define --------------------------------------*/
/* parse mark */
STATIC td_s32 jpeg_parse_fist_marker(jpeg_decompress_par_ptr par);
STATIC td_s32 jpeg_parse_next_marker(jpeg_decompress_par_ptr par);
STATIC td_s32 jpeg_parse_every_mark(jpeg_decompress_par_ptr par, td_s32 mark);
/* check image support */
STATIC td_s32 jpeg_parse_sofn_support(jpeg_decompress_par_ptr par, td_s32 mark);
STATIC td_s32 jpeg_parse_appn_support(jpeg_decompress_par_ptr par, td_s32 mark);
STATIC td_s32 jpeg_parse_rst_support(td_s32 mark);
STATIC td_s32 jpeg_parse_dax_support(td_s32 mark);
/* parse image information */
STATIC td_s32 jpeg_parse_header(jpeg_decompress_par_ptr par, td_s32 mark);
/* image sofn information */
STATIC td_s32 jpeg_parse_sofn(jpeg_decompress_par_ptr par);
STATIC td_s32 jpeg_parse_component_info(jpeg_decompress_par_ptr par);
STATIC td_s32 jpeg_parse_sample_fac(jpeg_decompress_par_ptr par);
STATIC td_s32 jpeg_parse_fmt(jpeg_decompress_par_ptr par);
/* image sos information */
STATIC td_s32 jpeg_parse_sos(jpeg_decompress_par_ptr par);
/* image dht information */
STATIC td_s32 jpeg_parse_dht(jpeg_decompress_par_ptr par);
/* image dqt information */
STATIC td_s32 jpeg_parse_dqt(jpeg_decompress_par_ptr par);
/* image dri information */
STATIC td_s32 jpeg_parse_dri(jpeg_decompress_par_ptr par);
/* image com information */
STATIC td_s32 jpeg_parse_com(jpeg_decompress_par_ptr par);
/* parse decompress information */
STATIC td_s32  jpeg_parse_decompress_info(jpeg_decompress_par_ptr par);
STATIC td_s32  jpeg_parse_calc_scale(jpeg_decompress_par_ptr par);
STATIC td_s32  jpeg_parse_calc_input_color_space(jpeg_decompress_par_ptr par);
STATIC td_void jpeg_parse_calc_output_color_space(jpeg_decompress_par_ptr par);
STATIC td_void jpeg_parse_calc_output_resolution(jpeg_decompress_par_ptr par);
STATIC td_void jpeg_parse_calc_output_wh(jpeg_decompress_par_ptr par);
STATIC td_void jpeg_parse_calc_output_mcuwh(jpeg_decompress_par_ptr par);
STATIC td_void jpeg_parse_calc_output_yuvwh(jpeg_decompress_par_ptr par);
STATIC td_void jpeg_parse_calc_output_stride(jpeg_decompress_par_ptr par);

/*-------------------------------------- func release -------------------------------------*/
td_u32 g_gainmap_offset = 0;
td_s32 jpeg_parse_start(jpeg_decompress_par_ptr par)
{
    td_s32 ret;
    td_s32 mark;

    ret = jpeg_parse_fist_marker(par);
    if (ret != EXT_SUCCESS) {
        gpu_err("call jpeg_parse_fist_marker failure");
        return EXT_FAILURE;
    }

    do {
        mark = jpeg_parse_next_marker(par);
        if (mark == (td_s32)EXT_FAILURE) {
            gpu_err("call jpeg_parse_next_marker failure");
            return EXT_FAILURE;
        }

        td_s32 parse_rslt = jpeg_parse_every_mark(par, mark);
        if (parse_rslt == JPEG_PARSE_FINISH) {
            break;
        }

        if (parse_rslt == JPEG_PARSE_ERROR) {
            gpu_err("parse failure");
            return EXT_FAILURE;
        }
    } while (TD_TRUE);

    ret = jpeg_parse_decompress_info(par);
    if (ret != EXT_SUCCESS) {
        gpu_err("call jpeg_parse_decompress_info failure");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 jpeg_parse_fist_marker(jpeg_decompress_par_ptr par)
{
    td_s32 code;
    td_bool skip_base_pic = par->hdr_en ? TD_TRUE : TD_FALSE;

    if (skip_base_pic == TD_TRUE) {
        jpeg_stream_skip_bytes(par, g_gainmap_offset);
        g_gainmap_offset = 0;
    }

    code = jpeg_stream_read_one_byte(par);
    if (code < 0) {
        gpu_err("call jpeg_stream_read_one_byte failure");
        return EXT_FAILURE;
    }

    if (code != MARK_TAG) {
        gpu_err("cannot parse SOI");
        return EXT_FAILURE;
    }

    code = jpeg_stream_read_one_byte(par);
    if (code < 0) {
        gpu_err("call jpeg_stream_read_one_byte failure");
        return EXT_FAILURE;
    }

    if (code != MARK_SOI) {
        gpu_err("cannot parse SOI");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 jpeg_parse_next_marker(jpeg_decompress_par_ptr par)
{
    td_s32 mark;

    do {
        mark = jpeg_stream_read_one_byte(par);
    } while ((mark != MARK_TAG) && (mark >= 0));

    do {
        mark = jpeg_stream_read_one_byte(par);
    } while (mark == MARK_TAG);

    return mark;
}

STATIC td_s32 jpeg_parse_every_mark(jpeg_decompress_par_ptr par, td_s32 mark)
{
    td_s32 ret;

    ret = jpeg_parse_sofn_support(par, mark);
    if (ret != EXT_SUCCESS) {
        gpu_err("call jpeg_parse_sofn_support failure");
        return JPEG_PARSE_ERROR;
    }

    ret = jpeg_parse_appn_support(par, mark);
    if (ret != EXT_SUCCESS) {
        gpu_err("call jpeg_parse_appn_support failure");
        return JPEG_PARSE_ERROR;
    }

    ret = jpeg_parse_rst_support(mark);
    if (ret != EXT_SUCCESS) {
        gpu_err("call jpeg_parse_rst_support failure");
        return JPEG_PARSE_ERROR;
    }

    ret = jpeg_parse_dax_support(mark);
    if (ret != EXT_SUCCESS) {
        gpu_err("call jpeg_parse_dax_support failure");
        return JPEG_PARSE_ERROR;
    }

    return jpeg_parse_header(par, mark);
}

STATIC td_s32 jpeg_parse_sofn_support(jpeg_decompress_par_ptr par, td_s32 mark)
{
    td_s32 len;

    par->arith_code = ((mark == MARK_SOF9) || (mark == MARK_SOF10)) ? (TD_TRUE) : (TD_FALSE);
    par->is_progressive = ((mark == MARK_SOF2) || (mark == MARK_SOF10)) ? (TD_TRUE) : (TD_FALSE);
    if ((par->arith_code == TD_TRUE) || (par->is_progressive == TD_TRUE)) {
        len = jpeg_stream_read_two_byte(par);
        len = (len >= JPEG_2BYTES) ? (len - JPEG_2BYTES) : 0;
        jpeg_stream_skip_bytes(par, (td_u32)len);
        gpu_err("is progressive");
        return EXT_FAILURE;
    }

    if ((mark == MARK_SOF3)  || (mark == MARK_SOF5)  ||
        (mark == MARK_SOF6)  || (mark == MARK_SOF7)  ||
        (mark == MARK_JPG)   || (mark == MARK_SOF11) ||
        (mark == MARK_SOF13) || (mark == MARK_SOF14) ||
        (mark == MARK_SOF15)) {
        len = jpeg_stream_read_two_byte(par);
        len = (len >= JPEG_2BYTES) ? (len - JPEG_2BYTES) : 0;
        jpeg_stream_skip_bytes(par, (td_u32)len);
        gpu_err("sof not support");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

#define EXTENED_INFO_LENGTH 16
#define MAX_HDR_IMAGE_CNT 2
#define BASE_IMAGE_TAG 0x01000000
#define GAINMAP_IMAGE_TAG1 0x02010000
#define GAINMAP_IMAGE_TAG2 0x02010100

#define check_return_value(ret) \
do { \
    if ((ret) != EXT_SUCCESS) { \
        gpu_print("LINE:%d, FUNC:%s failed\n", __LINE__, __FUNCTION__); \
        return EXT_FAILURE; \
    } \
} while (0)

STATIC td_s32 read_bytes(jpeg_decompress_par_ptr par, td_u32 *code, td_u32 bytes_cnt, td_u32 *left_len)
{
    if (bytes_cnt > *left_len) {
        gpu_dbg("read bytes failed, bytes_cnt:%d left_len:%d\n", bytes_cnt, *left_len);
        return EXT_FAILURE;
    }
    if (bytes_cnt == JPEG_1BYTE) {
        *code = (td_u32)jpeg_stream_read_one_byte(par);
    } else if (bytes_cnt == JPEG_2BYTES) {
        *code = (td_u32)jpeg_stream_read_two_byte(par);
    } else if (bytes_cnt == JPEG_4BYTES) {
        *code = (td_u32)jpeg_stream_read_four_byte(par);
    } else {
        gpu_dbg("read bytes failed, bytes_cnt:%d\n", bytes_cnt);
        return EXT_FAILURE;
    }
    *left_len -= bytes_cnt;
    return EXT_SUCCESS;
}

#define try_skip_bytes(par, bytes_cnt, left_len) \
do { \
    if ((bytes_cnt) > (left_len)) { \
        return EXT_FAILURE; \
    } \
    jpeg_stream_skip_bytes(par, (td_u32)(bytes_cnt)); \
    (left_len) -= (bytes_cnt); \
} while (0)

/********************** UWA Info APP8 Structure ***************************
b(8)   A byte of any value. The parsing process is specified by the return value of the read_bits(8) function.
f(n)   takes n consecutive binary bits of a specific value.
       The parsing process is specified by the return value of the read_bits(n) function.
r(n)   n consecutive '0's. The parsing process is specified by the return value of the read_bits(n) function.
u(n)   n-bit unsigned integer. In the syntax table, if n is "v",
       the number of bits is determined by the other syntax element values. The parsing process consists of functions
i(n)   n-bit signed integer
st(n)  n characters in UTF-8 format, complying with ISO/IEC 10646.
fp(n)  indicates an n-bit floating point number, which complies with ISO/IEC 60559:2020 (IEEE 754).
       The value of n can be 16 or 32.
ur(64) uses two 32-bit unsigned numbers, val1 and val2, and the value of the variable is val1/val2.
ir(64) uses a 32-bit signed number val1 and a 32-bit unsigned number val2. The value of the variable is val1/val2.

BASE Image {
    app_start_code f(16)
    app_length u(16)
    itut35_tag st(6)
    itu_t_t35_country_code f(8)
    terminal_provide_code f(16)
    terminal_provide_oriented_code f(16)
    extended_frame_number u(8)
    extended_type f(32)
    if(File_type==1){
        for(i=0; i < ImageNumber-1; i++){
        extended_offset1[i] u(32)
        extended_offset2[i] u(32)
    }
}

Gainmap Image( ) {
    app_start_code f(16)
    app_length u(16)
    itut35_tag st(6)
    itu_t_t35_country_code f(8)
    terminal_provide_code f(16)
    terminal_provide_oriented_code f(16)
    extended_frame_number f(8)
    extended_type f(32)
    if(meta_type==1){
        metedata_size u(16)
        static_metedata_size u(16)
        static_metadata u(8)×N
        dynamic_metedata_size u(16)
        dynamic_metedata u(8)×N
    }
    if(enhance_type==1){
        extend_info_size u(16)
        extend_info_extention u(8)×N
    }
}

Extended_type
0x01000000: Base Image Type
0x02010100 / 0x02010000 : Gainmap Image Type
}
********************** UWA Info APP8 Structure ***************************/

/********************* METADATA Structure ***************************
staic metadata() {
    for(c=0;c<3;c++){
        display_primaries_x[c] u(16)
        display_primaries_y[c] u(16)
    }
    white_point_x u(16)
    white_point_y u(16)
    max_display_mastering_luminance u(16)
    min_display_mastering_luminance u(16)
    max_content_light_level u(16)
    max_picture_average_light_level u(16)
}


dynamic metadata() {
    system_start_code u(8)
    for(w=0;w<num_windows;w++){
        minimum_maxrgb_pq[w] u(12)
        average_maxrgb_pq[w] u(12)
        variance_maxrgb_pq[w] u(12)
        maximum_maxrgb_pq[w] u(12)
    }
    ...
    ...
    ...
}
**********************  METADATA Structure **************************/
#ifdef GPU_HDR_METADATA_VALID
STATIC td_s32 read_bits(jpeg_decompress_par_ptr par, td_u32 bits_num, td_u32 *len, td_u32 is_reset)
{
    td_s32 result = 0;
    static td_u32 buffer = 0;
    static td_s32 bits_remaining = 0;

    if (is_reset) {
        buffer = 0;
        bits_remaining = 0;
    }
    while (bits_num > 0) {
        if (bits_remaining == 0) {
            check_return_value(read_bytes(par, &buffer, 1, len));
            bits_remaining = 0x8; /* 8 bits per byte */
        }
        td_s32 bit = (buffer >> (bits_remaining - 1)) & 1;
        result = (result << 1) | bit;
        bits_remaining--;
        bits_num--;
    }

    return result;
}

STATIC td_s32 parse_gainmap_static_metadata(jpeg_decompress_par_ptr par, td_u32 *len)
{
    td_u32 code;
    hdr_static_metadata *info = &par->hdr_attr.metadata_standard.static_metadata;

    for (int i = 0; i < 0x3; i++) { /* component maximum is 3 */
        check_return_value(read_bytes(par, &code, JPEG_2BYTES, len)); /* display_primaries_x */
        info->display_primaries_x[i] = code;
        check_return_value(read_bytes(par, &code, JPEG_2BYTES, len)); /* display_primaries_y */
        info->display_primaries_y[i] = code;
    }
    check_return_value(read_bytes(par, &code, JPEG_2BYTES, len)); /* white_point_x */
    info->white_point_x = code;
    check_return_value(read_bytes(par, &code, JPEG_2BYTES, len)); /* white_point_y */
    info->white_point_y = code;
    check_return_value(read_bytes(par, &code, JPEG_4BYTES, len)); /* max_display_mastering_luminance */
    info->max_display_mastering_luminance = code;
    check_return_value(read_bytes(par, &code, JPEG_4BYTES, len)); /* min_display_mastering_luminance */
    info->min_display_mastering_luminance = code;
    check_return_value(read_bytes(par, &code, JPEG_2BYTES, len)); /* max_content_light_level */
    info->max_content_light_level = code;
    check_return_value(read_bytes(par, &code, JPEG_2BYTES, len)); /* max_picture_average_light_level */
    info->max_frame_average_light_level = code;

    return EXT_SUCCESS;
}

STATIC td_s32 parse_gainmap_dynamic_metadata(jpeg_decompress_par_ptr par, td_u32 *len)
{
    hdr_dynamic_metadata *info = &par->hdr_attr.metadata_standard.dynamic_metadata;
    info->system_start_code = read_bits(par, 8, len, 0); // u(8)

    if (info->system_start_code != 1) {
        return EXT_SUCCESS;
    }

    info->minimum_maxrgb_pq  = read_bits(par, 12, len, 0); // u(12)
    info->average_maxrgb_pq  = read_bits(par, 12, len, 0); // u(12)
    info->variance_maxrgb_pq = read_bits(par, 12, len, 0); // u(12)
    info->maximum_maxrgb_pq  = read_bits(par, 12, len, 0); // u(12)

    info->tone_mapping_enable_mode_flag = read_bits(par, 1, len, 0);     // u(1)
    if (info->tone_mapping_enable_mode_flag != 0) {
        info->tone_mapping_param_enable_num = read_bits(par, 1, len, 0); // u(1)
        for (td_u32 i = 0; i < info->tone_mapping_param_enable_num + 1; i++) {
            info->targeted_system_display_maximum_luminance_pq[i] = read_bits(par, 12, len, 0); // u(12)
            info->base_enable_flag[i] = read_bits(par, 1, len, 0);                              // u(1)
            if (info->base_enable_flag[i] != 0) {
                info->base_param_m_p[i] = read_bits(par, 14, len, 0); // u(14)
                info->base_param_m_m[i] = read_bits(par, 6, len, 0);  // u(6)
                info->base_param_m_a[i] = read_bits(par, 10, len, 0); // u(10)
                info->base_param_m_b[i] = read_bits(par, 10, len, 0); // u(10)
                info->base_param_m_n[i] = read_bits(par, 6, len, 0);  // u(6)
                info->base_param_k1[i]  = read_bits(par, 2, len, 0);  // u(2)
                info->base_param_k2[i]  = read_bits(par, 2, len, 0);  // u(2)
                info->base_param_k3[i]  = read_bits(par, 4, len, 0);  // u(4)
                info->base_param_delta_enable_mode[i] = read_bits(par, 3, len, 0); // u(3)
                info->base_param_enable_delta[i]      = read_bits(par, 7, len, 0); // u(7)
            }

            info->p_3spline_enable_flag[i] = read_bits(par, 1, len, 0);    // u(1)
            if (info->p_3spline_enable_flag[i] != 0) {
                info->p_3spline_enable_num[i] = read_bits(par, 1, len, 0); // u(1)
                for (td_u32 j = 0; j < info->p_3spline_enable_num[i] + 1; j++) {
                    info->p_3spline_th_enable_mode[j][i] = read_bits(par, 2, len, 0);   // u(2)
                    if (info->p_3spline_th_enable_mode[j][i] == 0 ||
                        info->p_3spline_th_enable_mode[j][i] == 2) { // 2:index
                        info->p_3spline_th_enable_mb[j][i] = read_bits(par, 8, len, 0); // f(8)
                    }
                    info->p_3spline_th_enable[j][i]        = read_bits(par, 12, len, 0); // f(12)
                    info->p_3spline_th_enable_delta1[j][i] = read_bits(par, 10, len, 0); // f(10)
                    info->p_3spline_th_enable_delta2[j][i] = read_bits(par, 10, len, 0); // f(10)
                    info->p_3spline_enable_strength[j][i]  = read_bits(par, 8, len, 0);  // f(8)
                }
            }
        }
    }

    info->color_saturation_mapping_enable_flag = read_bits(par, 1, len, 0);    // u(1)
    if (info->color_saturation_mapping_enable_flag != 0) {
        info->color_saturation_enable_num = read_bits(par, 3, len, 0);         // u(3)
        for (td_u32 i = 0; i < info->color_saturation_enable_num; i++) {
            info->color_saturation_enable_gain[i] = read_bits(par, 8, len, 0); // u(8)
        }
    }

    read_bits(par, 0, len, 1);

    return EXT_SUCCESS;
}
#endif

STATIC td_s32 parse_gainmap_metadata(jpeg_decompress_par_ptr par, td_u32 *len)
{
    td_u32 code;

    /* parse metadata */
    check_return_value(read_bytes(par, &code, JPEG_2BYTES, len));
#ifdef GPU_HDR_METADATA_VALID
    check_return_value(read_bytes(par, &code, JPEG_2BYTES, len)); /* static metadata size */
    parse_gainmap_static_metadata(par, len);
    check_return_value(read_bytes(par, &code, JPEG_2BYTES, len)); /* dynamic metadata size */
    parse_gainmap_dynamic_metadata(par, len);
#else
    try_skip_bytes(par, code, *len);
#endif
    if (code == GAINMAP_IMAGE_TAG2) {
        /* parse extend_info_extention */
        check_return_value(read_bytes(par, &code, JPEG_2BYTES, len));
        try_skip_bytes(par, code, code); /* extend_info extension size */
    }
    return EXT_SUCCESS;
}

STATIC td_s32 parse_uwa_info(jpeg_decompress_par_ptr par)
{
    td_u32 code, len, extended_frame_number;
    td_char code_array[0x6] = { 0 }; /* itut35_tag with 6 bytes */
    td_s32 ret = EXT_FAILURE;

    par->hdr_attr.image_cnt = 1;
    td_s32 code_tmp = jpeg_stream_read_two_byte(par);
    if (code_tmp == -1) {
        gpu_dbg("jpeg_stream_read_two_byte failure\n");
        return EXT_FAILURE;
    }
    code = (td_u32)code_tmp;
    len = (code >= JPEG_2BYTES) ? (code - JPEG_2BYTES) : 0;

    if (len < EXTENED_INFO_LENGTH) {
        goto SKIP_BYTES;
    }

    check_return_value(jpeg_stream_read_multi_bytes(par, code_array, 0x6)); /* itut35_tag with 6 bytes */
    if ((code_array[0x0] != 'I') || (code_array[0x1] != 'T') || (code_array[0x2] != 'U') ||
        (code_array[0x3] != 'T') || (code_array[0x4] != '3') || (code_array[0x5] != '5')) {
        len -= 0x6;
        goto SKIP_BYTES;
    }
    try_skip_bytes(par, 0x5, len);
    check_return_value(read_bytes(par, &extended_frame_number, JPEG_1BYTE, &len));
    gpu_dbg("image_cnt:%d\n", extended_frame_number);
    /* HDR_JPEG image consists of base image and gainmap image */
    if (extended_frame_number == 0 || extended_frame_number > MAX_HDR_IMAGE_CNT) {
        goto SKIP_BYTES;
    }
    par->hdr_attr.image_cnt = (td_u8)extended_frame_number;

    check_return_value(read_bytes(par, &code, JPEG_4BYTES, &len));
    switch (code) {
        case BASE_IMAGE_TAG:
            for (td_u32 i = 0; i < extended_frame_number - 1; i++) {
                check_return_value(read_bytes(par, &code, JPEG_4BYTES, &len)); /* gainmap offset */
                /* there is no multithreading decoding scene for now, no need to lock */
                g_gainmap_offset = par->src.block_offset + par->src.cur_offset + code - JPEG_4BYTES;
                gpu_dbg("gainmap_offset:%d\n", g_gainmap_offset);
                check_return_value(read_bytes(par, &code, JPEG_4BYTES, &len));
            }
            break;
        case GAINMAP_IMAGE_TAG1:
        case GAINMAP_IMAGE_TAG2:
            parse_gainmap_metadata(par, &len);
            break;
        default:
            break;
    }
    ret = EXT_SUCCESS;
SKIP_BYTES:
    try_skip_bytes(par, len, len);
    return ret;
}

/********************** CICP APP2 Structure ***************************
Base/Gainmap Image( ) {
    cicp_tag  f(32)
    reserved code u(32)
    ColourPrimaries u(8)
    TransferCharacteristics u(8)
    MatrixCoefficients u(8)
    VideoFullRangeFlag u(8)
}

ColourPrimaries
1: indicating that the three primary color coordinates and white points of SCAN/sYCC are used.
9: indicating that the tricolor coordinates and white points of BT.2020/BT.2100 are used.

TransferCharacteristics
13: indicating that RGB/sYCC is used.
16: indicating that PQ is used.
18: indicating that HLG is used.

MatrixCoefficients
5: indicating that the sYCC conversion matrix is used.
9: indicating that the BT.2020/BT.2100 conversion matrix is used.
********************** CICP APP2 Structure ***************************/
STATIC td_s32 parse_cicp(jpeg_decompress_par_ptr par, td_u32 len)
{
    td_u32 code;
    td_char cicp_tag[0x8] = {'c', 'i', 'c', 'p', 0, 0, 0, 0};
    td_u8 hit_index = 0;
    td_u8 max = 0;

    while (len >= 0 && hit_index < 0x8) {
        check_return_value(read_bytes(par, &code, JPEG_1BYTE, &len));
        if ((td_char)code != cicp_tag[hit_index]) {
            hit_index = 0;
        } else {
            hit_index++;
            max = (max < hit_index) ? hit_index : max;
        }
    }
    if (hit_index == 0x8 && len >= 0x4) {
        check_return_value(read_bytes(par, (td_u32 *)&par->hdr_attr.cicp_info.color_primaries, JPEG_1BYTE, &len));
        check_return_value(read_bytes(par, (td_u32 *)&par->hdr_attr.cicp_info.transfer_characteristics,
            JPEG_1BYTE, &len));
        check_return_value(read_bytes(par, (td_u32 *)&par->hdr_attr.cicp_info.matrix_coefficients, JPEG_1BYTE, &len));
        check_return_value(read_bytes(par, (td_u32 *)&par->hdr_attr.cicp_info.video_full_range_flag, JPEG_1BYTE, &len));
    }

    gpu_dbg("cicp:color_primaries[%d], transfer_characteristics[%d], matrix_coef[%d], video_full_range_flag[%d]\n",
        par->hdr_attr.cicp_info.color_primaries, par->hdr_attr.cicp_info.transfer_characteristics,
        par->hdr_attr.cicp_info.matrix_coefficients, par->hdr_attr.cicp_info.video_full_range_flag);

    try_skip_bytes(par, len, len);
    return EXT_SUCCESS;
}

/********************** ISO APP2 Structure ***************************
BASE IMAGE:
ISOMetadata APP{
    app_start_code f(16) app_length u(16)
    iso_gainmap_tag st(28)
    payload f(32)
}

GAINMAP IMAGE:
ISOMetadata APP{
    app_start_code f(16)
    app_length u(16)
    iso_gainmap_tag st(28)
    version u(32)
    if(minimum_version==0){
        flag u(8)
        base_headroom_numerator u(32)
        base_headroom_denominator u(32)
        alternate_headroom_numerator u(32)
        alternate_headroom_denominator u(32)
        for(i=0; i < number_of_components-1; i++){
            extend_info_main[i]
        }
    }
}
minimum_version: version & 0xFF；
number_of_components: (flag>>7)*2+1

extend_info_main{
    per_component_min_gain_map_numerator i(32)
    per_component_min_gain_map_denominator u(32)
    per_component_max_gain_map_numerator i(32)
    per_component_max_gain_map_denominator u(32)
    Per-component gamma values_numerator u(32)
    Per-component gamma values_denominator u(32)
    baseline_image_offset_numerator i(32)
    baseline_image_offset_denominator u(32)
    alternate_image_offset_numerator i(32)
    alternate_image_offset_denominator u(32)
}
********************** ISO APP2 Structure ***************************/

STATIC td_void get_extend_info_main(td_s32 *extend_info_main_info, hdr_extend_info *info, td_u32 component_index)
{
    if (extend_info_main_info[1] != 0) {
        info->min_gainmap[component_index] = (td_float)extend_info_main_info[0] / (td_float)extend_info_main_info[1];
    }

    if (extend_info_main_info[0x3] != 0) {
        info->max_gainmap[component_index] = (td_float)extend_info_main_info[0x2] /
            (td_float)extend_info_main_info[0x3];
    }

    if (extend_info_main_info[0x5] != 0) {
        info->gamma_values[component_index] = (td_float)extend_info_main_info[0x4] /
            (td_float)extend_info_main_info[0x5];
    }

    if (extend_info_main_info[0x7] != 0) {
        info->baseline_image_offset[component_index] = (td_float)extend_info_main_info[0x6] /
            (td_float)extend_info_main_info[0x7];
    }

    if (extend_info_main_info[0x9] != 0) {
        info->alternate_image_offset[component_index] = (td_float)extend_info_main_info[0x8] /
            (td_float)extend_info_main_info[0x9];
    }
    gpu_dbg("min_gainmap[%d]:%x/%x = %f\n", component_index, extend_info_main_info[0],
        extend_info_main_info[1], info->min_gainmap[component_index]);
    gpu_dbg("max_gainmap[%d]:%x/%x = %f\n", component_index, extend_info_main_info[0x2],
        extend_info_main_info[0x3], info->max_gainmap[component_index]);
    gpu_dbg("gamma_values[%d]:%x/%x = %f\n", component_index, extend_info_main_info[0x4],
        extend_info_main_info[0x5], info->gamma_values[component_index]);
    gpu_dbg("baseline_image_offset[%d]:%x/%x = %f\n", component_index, extend_info_main_info[0x6],
        extend_info_main_info[0x7], info->baseline_image_offset[component_index]);
    gpu_dbg("alternate_image_offset[%d]:%x/%x = %f\n", component_index, extend_info_main_info[0x8],
        extend_info_main_info[0x9], info->alternate_image_offset[component_index]);
}

STATIC td_s32 parse_iso_info(jpeg_decompress_par_ptr par, td_u32 len)
{
    td_u32 code, code1;
    td_u32 components_cnt;
    td_s32 extend_info_main_info[10]; /* 10 parameters for extend_info_main numerator */
    hdr_extend_info *info = &par->hdr_attr.extend_info;
    td_s32 ret = EXT_FAILURE;

    if (par->hdr_en == TD_FALSE) {
        /* only gainmap image contain extend_info */
        goto SKIP_APP;
    }

    if (len < 32) { /* 32 is len maximum */
        goto SKIP_APP;
    }

    check_return_value(read_bytes(par, &code, JPEG_4BYTES, &len));
    if ((code & 0xff) == 0) {
        /* parse extend_info */
        check_return_value(read_bytes(par, &code, JPEG_1BYTE, &len));
        components_cnt = ((code >> 0x7) & 0x1) * 0x2 + 1;
        if (components_cnt > 0x3) {
            goto SKIP_APP;
        }
        gpu_dbg("components_cnt: %x -> %d\n", code, components_cnt);
        /* parse base headroom and alternate_headroom info */
        check_return_value(read_bytes(par, &code, JPEG_4BYTES, &len));
        check_return_value(read_bytes(par, &code1, JPEG_4BYTES, &len));
        info->base_headroom = (td_float)code / (td_float)code1;
        gpu_dbg("base_headroom:%x/%x=%f\n", code, code1, info->base_headroom);
        check_return_value(read_bytes(par, &code, JPEG_4BYTES, &len));
        check_return_value(read_bytes(par, &code1, JPEG_4BYTES, &len));
        info->alternate_headroom = (td_float)code / (td_float)code1;
        gpu_dbg("alternate_headroom:%x/%x=%f\n", code, code1, info->alternate_headroom);
        for (td_u32 i = 0; i < components_cnt; i++) {
            /* parse_extend_info_main */
            for (td_u32 j = 0; j < 10; j++) { /* 10 parameters for extend_info_main */
                check_return_value(read_bytes(par, (td_u32 *)&extend_info_main_info[j], JPEG_4BYTES, &len));
            }
            get_extend_info_main(extend_info_main_info, info, i);
        }
    }
    ret = EXT_SUCCESS;
SKIP_APP:
    try_skip_bytes(par, len, len);
    return ret;
}

STATIC td_void parse_mpf_info(jpeg_decompress_par_ptr par, td_s32 len)
{
    jpeg_stream_skip_bytes(par, (td_u32)len);
    return;
}
#define APP2_MPF 0
#define APP2_ICC 1
#define APP2_ISO 2
#define APP2_MPF_LEN 3
#define APP2_ISO_LEN 28
#define APP2_ICC_LEN 11
#define APP2_INVAILD (-1)
STATIC td_s32 parse_app2_tag(jpeg_decompress_par_ptr par, td_s32 *app_len)
{
    td_s32 code;
    td_u8 MPF_tag[APP2_MPF_LEN] = {'M', 'P', 'F'};
    td_u8 ISO_tag[APP2_ISO_LEN] = {'u', 'r', 'n', ':', 'i', 's', 'o', ':', 's', 't', 'd', ':', 'i', 's', 'o', ':',
        't', 's', ':', '2', '1', '4', '9', '6', ':', '-', '1', 0};
    td_u8 ICC_tag[APP2_ICC_LEN] = {'I', 'C', 'C', '_', 'P', 'R', 'O', 'F', 'I', 'L', 'E'};
    td_u32 index = 0;

    if (*app_len < 32) { /* 32 is len maximum */
        goto SKIP_APP;
    }

    code = jpeg_stream_read_one_byte(par);
    if (code == MPF_tag[index]) {
        do {
            index++;
            code = jpeg_stream_read_one_byte(par);
            if (code != MPF_tag[index]) {
                index++;
                goto SKIP_APP;
            }
        } while (index < (APP2_MPF_LEN - 1));
        *app_len = *app_len - APP2_MPF_LEN;
        return APP2_MPF;
    } else if (code == ICC_tag[index]) {
        do {
            index++;
            code = jpeg_stream_read_one_byte(par);
            if (code != ICC_tag[index]) {
                index++;
                goto SKIP_APP;
            }
        } while (index < (APP2_ICC_LEN - 1));
        *app_len = *app_len - APP2_ICC_LEN;
        return APP2_ICC;
    } else if (code == ISO_tag[index]) {
        do {
            index++;
            code = jpeg_stream_read_one_byte(par);
            if (code != ISO_tag[index]) {
                index++;
                goto SKIP_APP;
            }
        } while (index < (APP2_ISO_LEN - 1));
        *app_len = *app_len - APP2_ISO_LEN;
        return APP2_ISO;
    }

SKIP_APP:
    jpeg_stream_skip_bytes(par, *app_len - index);
    return APP2_INVAILD;
}

STATIC td_s32 jpeg_parse_appn_support(jpeg_decompress_par_ptr par, td_s32 mark)
{
    td_s32 len;
    td_s32 app2_tag;

    if (mark == MARK_APP2) {
        len = jpeg_stream_read_two_byte(par);
        len = (len >= JPEG_2BYTES) ? (len - JPEG_2BYTES) : 0;
        app2_tag = parse_app2_tag(par, &len);
        if (app2_tag == APP2_MPF) {
            parse_mpf_info(par, len);
        } else if (app2_tag == APP2_ICC) {
            parse_cicp(par, (td_u32)len);
        } else if (app2_tag == APP2_ISO) {
            parse_iso_info(par, (td_u32)len);
        } else {
            gpu_err("invalid app2\n");
        }
    } else if (mark == MARK_APP8) {
        parse_uwa_info(par);
    } else if ((mark == MARK_APP0 || mark >= MARK_APP3) && (mark <= MARK_APP15)) {
        len = jpeg_stream_read_two_byte(par);
        len = (len >= JPEG_2BYTES) ? (len - JPEG_2BYTES) : 0;
        jpeg_stream_skip_bytes(par, (td_u32)len);
    }
    return EXT_SUCCESS;
}

STATIC td_s32 jpeg_parse_rst_support(td_s32 mark)
{
    if ((mark == MARK_RST0) || (mark == MARK_RST1) ||
        (mark == MARK_RST2) || (mark == MARK_RST3) ||
        (mark == MARK_RST4) || (mark == MARK_RST5) ||
        (mark == MARK_RST6) || (mark == MARK_RST7)) {
        gpu_err("rst not support");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

STATIC td_s32 jpeg_parse_dax_support(td_s32 mark)
{
    if ((mark == MARK_DAC) || (mark == MARK_TEM) || (mark == MARK_DNL)) {
        gpu_err("dax not support");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

STATIC td_void jpeg_parse_sos_resume_file(jpeg_decompress_par_ptr par)
{
    if (par->src.is_mem == TD_FALSE) {
        fseek(par->src.infile, par->src.cur_offset, SEEK_SET);
    }
}

STATIC td_s32 jpeg_parse_header(jpeg_decompress_par_ptr par, td_s32 mark)
{
    switch (mark) {
        case MARK_SOF0:
        case MARK_SOF1:
            if (jpeg_parse_sofn(par) != EXT_SUCCESS) {
                gpu_err("call jpeg_parse_sofn failure");
                return JPEG_PARSE_ERROR;
            }
            break;
        case MARK_APP1:
            jpeg_exif_parse(par);
            break;
        case MARK_SOS:
            if (jpeg_parse_sos(par) != EXT_SUCCESS) {
                gpu_err("call jpeg_parse_sos failure");
                return JPEG_PARSE_ERROR;
            }
            jpeg_parse_sos_resume_file(par);
            return JPEG_PARSE_FINISH;
        case MARK_DHT:
            if (jpeg_parse_dht(par) != EXT_SUCCESS) {
                gpu_err("call jpeg_parse_dht failure");
                return JPEG_PARSE_ERROR;
            }
            break;
        case MARK_DQT:
            if (jpeg_parse_dqt(par) != EXT_SUCCESS) {
                gpu_err("call jpeg_parse_dqt failure");
                return JPEG_PARSE_ERROR;
            }
            break;
        case MARK_DRI:
            if (jpeg_parse_dri(par) != EXT_SUCCESS) {
                gpu_err("call jpeg_parse_dri failure");
                return JPEG_PARSE_ERROR;
            }
            break;
        case MARK_COM:
            if (jpeg_parse_com(par) != EXT_SUCCESS) {
                gpu_err("call jpeg_parse_com failure");
                return JPEG_PARSE_ERROR;
            }
            break;
        case MARK_EOI:
            return JPEG_PARSE_FINISH;
        default:
            return JPEG_PARSE_CONTINUE;
    }

    return JPEG_PARSE_CONTINUE;
}

STATIC td_s32 jpeg_parse_sofn(jpeg_decompress_par_ptr par)
{
    td_s32 data_precision, len;

    len = jpeg_stream_read_two_byte(par);
    data_precision = jpeg_stream_read_one_byte(par);
    if (data_precision != JPEG_DATA_PRECISION) {
        gpu_err("data precision is not support");
        return EXT_FAILURE;
    }

    par->image_height   = (td_u32)jpeg_stream_read_two_byte(par);
    par->image_width    = (td_u32)jpeg_stream_read_two_byte(par);
    par->num_components = (td_u8)jpeg_stream_read_one_byte(par);

    len -= JPEG_8BYTES;
    if ((par->num_components * JPEG_YUV_COMPONENTS) != len) {
        gpu_err("component not support");
        return EXT_FAILURE;
    }

    if ((par->num_components > JPEG_MAX_COMPONENTS) ||
        (par->image_width < JPEG_MIN_WIDTH) || (par->image_height < JPEG_MIN_HEIGHT) ||
        (par->image_width > JPEG_MAX_WIDTH) || (par->image_height > JPEG_MAX_HEIGHT)) {
        gpu_err("size is not support");
        return EXT_FAILURE;
    }

    if (jpeg_parse_component_info(par) != EXT_SUCCESS) {
        gpu_err("call jpeg_parse_component_info failure");
        return EXT_FAILURE;
    }

    if (jpeg_parse_sample_fac(par) != EXT_SUCCESS) {
        gpu_err("call jpeg_parse_sample_fac failure");
        return EXT_FAILURE;
    }

    if (jpeg_parse_fmt(par) != EXT_SUCCESS) {
        gpu_err("call jpeg_parse_fmt failure");
        return EXT_FAILURE;
    }

    par->has_sof = TD_TRUE;

    return EXT_SUCCESS;
}

STATIC td_s32 jpeg_parse_component_info(jpeg_decompress_par_ptr par)
{
    td_s32 i;
    td_u32 code;
    td_s32 max_h_samp_factor = 1;
    td_s32 max_v_samp_factor = 1;

    for (i = 0; i < par->num_components; i++) {
        par->component_info[i].component_index = i;
        par->component_info[i].component_id = jpeg_stream_read_one_byte(par);

        code = (td_u32)jpeg_stream_read_one_byte(par);
        par->component_info[i].h_samp_factor = (code >> JPEG_SHIFT_4BITS) & 0xf;
        par->component_info[i].v_samp_factor = code & 0xf;

        par->component_info[i].quant_table_no = jpeg_stream_read_one_byte(par);

        if ((par->component_info[i].h_samp_factor <= 0) ||
            (par->component_info[i].h_samp_factor > JPEG_MAX_SAMP_FACTOR) ||
            (par->component_info[i].v_samp_factor <= 0) ||
            (par->component_info[i].v_samp_factor > JPEG_MAX_SAMP_FACTOR)) {
            gpu_err("sample factor not support");
            return EXT_FAILURE;
        }

        max_h_samp_factor = jpeg_max(max_h_samp_factor, par->component_info[i].h_samp_factor);
        max_v_samp_factor = jpeg_max(max_v_samp_factor, par->component_info[i].v_samp_factor);
    }

    return EXT_SUCCESS;
}

STATIC td_s32 jpeg_parse_sample_fac(jpeg_decompress_par_ptr par)
{
    td_u8 component_y_h = par->component_info[0].h_samp_factor;
    td_u8 component_y_v = par->component_info[0].v_samp_factor;
    td_u8 component_cb_h = par->component_info[1].h_samp_factor;
    td_u8 component_cb_v = par->component_info[1].v_samp_factor;
    td_u8 component_cr_h = par->component_info[2].h_samp_factor;
    td_u8 component_cr_v = par->component_info[2].v_samp_factor;

    if (par->num_components == 1) {
        if (component_y_h == component_y_v) {
            par->reg_mgr.y_fac = (td_u8)(((1 << 4) | 1) & 0xff); /* shift 4 for h, no shift for v */
        }
        return EXT_SUCCESS;
    }

    if ((par->num_components == JPEG_YUV_COMPONENTS) &&
        (component_cb_h == component_cr_h) &&
        (component_cb_v == component_cr_v)) {
        par->reg_mgr.y_fac = ((component_y_h  << 4) | component_y_v)  & 0xff; /* index: 4 */
        par->reg_mgr.u_fac = ((component_cb_h << 4) | component_cb_v) & 0xff; /* index: 4 */
        par->reg_mgr.v_fac = ((component_cr_h << 4) | component_cr_v) & 0xff; /* index: 4 */
        return EXT_SUCCESS;
    }

    return EXT_FAILURE;
}

STATIC td_s32 jpeg_parse_fmt(jpeg_decompress_par_ptr par)
{
    td_s32 component_cb_h = par->component_info[1].h_samp_factor;
    td_s32 component_cb_v = par->component_info[1].v_samp_factor;
    td_s32 component_cr_h = par->component_info[2].h_samp_factor;
    td_s32 component_cr_v = par->component_info[2].v_samp_factor;

    if (par->num_components == 1) {
        if (par->component_info[0].h_samp_factor == par->component_info[0].v_samp_factor) {
            par->image_color_space = MY_JPEG_COLOR_SPACE_YUV400;
            return EXT_SUCCESS;
        }
    }

    if ((par->num_components != JPEG_YUV_COMPONENTS) || (component_cb_h != component_cr_h) ||
        (component_cb_v != component_cr_v)) {
        par->image_color_space = MY_JPEG_COLOR_SPACE_MAX;
        gpu_err("color space is not support");
        return EXT_FAILURE;
    }

    if (par->component_info[0].h_samp_factor == ((par->component_info[1].h_samp_factor) << 1)) {
        if (par->component_info[0].v_samp_factor == ((par->component_info[1].v_samp_factor) << 1)) {
            par->image_color_space = MY_JPEG_COLOR_SPACE_YUV420;
            return EXT_SUCCESS;
        } else if (par->component_info[0].v_samp_factor == par->component_info[1].v_samp_factor) {
            par->image_color_space = MY_JPEG_COLOR_SPACE_YUV422_21;
            return EXT_SUCCESS;
        }
    } else if (par->component_info[0].h_samp_factor == par->component_info[1].h_samp_factor) {
        if (par->component_info[0].v_samp_factor == ((par->component_info[1].v_samp_factor) << 1)) {
            par->image_color_space = MY_JPEG_COLOR_SPACE_YUV422_12;
            return EXT_SUCCESS;
        } else if (par->component_info[0].v_samp_factor == par->component_info[1].v_samp_factor) {
            par->image_color_space = MY_JPEG_COLOR_SPACE_YUV444;
            return EXT_SUCCESS;
        }
    }

    par->image_color_space = MY_JPEG_COLOR_SPACE_MAX;

    gpu_err("color space is not support");
    return EXT_FAILURE;
}

STATIC td_s32 jpeg_parse_sos(jpeg_decompress_par_ptr par)
{
    td_s32 len, num_component, i, j, code, code1;
    td_u32 ss, se, ah, al;

    if (par->has_sof != TD_TRUE) {
        return EXT_SUCCESS;
    }

    len = jpeg_stream_read_two_byte(par);
    num_component = jpeg_stream_read_one_byte(par);
    if ((len != (num_component * 2 + 6)) || (num_component < 1) || /* 2,6:index */
        (num_component > JPEG_MAX_COMPONENTS) || (par->num_components > JPEG_MAX_COMPONENTS)) {
        gpu_err("sos not support");
        return EXT_FAILURE;
    }

    par->components_in_scan = (td_u8)num_component;

    for (i = 0; i < num_component; i++) {
        code1 = jpeg_stream_read_one_byte(par);
        code = jpeg_stream_read_one_byte(par);
        for (j = 0; j < par->num_components; j++) {
            if (code1 == par->component_info[j].component_id) {
                goto id_found;
            }
        }
        return EXT_FAILURE;
        id_found:
            par->component_info[j].dc_table_no = ((td_u32)code >> 4) & 15; /* 4 shif val, 15 index */
            par->component_info[j].ac_table_no = (td_u32)code & 15;        /* 15 index */
    }

    ss = (td_u32)jpeg_stream_read_one_byte(par);
    se = (td_u32)jpeg_stream_read_one_byte(par);
    code = jpeg_stream_read_one_byte(par);
    ah = ((td_u32)code >> 4) & 15; /* 4 shif val, 15 index */
    al = (td_u32)code & 15;        /* 15 index */

    code = (td_s32)(ss | se | ah | al); /* there is no point */

    return EXT_SUCCESS;
}

STATIC td_s32 jpeg_parse_dht(jpeg_decompress_par_ptr par)
{
    td_s32 err_ret, len, i, count, hor_table_index;
    td_u8 bits[JPEG_HUFF_BITS_SIZE] = {0};
    td_u8 huffval[JPEG_HUFF_CODE_SIZE] = {0};
    my_jpeg_huff_table *htblptr = TD_NULL;

    len = jpeg_stream_read_two_byte(par);
    len -= 2; /* 2: index */

    while (len > JPEG_HUFF_LENGTH) {
        hor_table_index = jpeg_stream_read_one_byte(par);
        count = bits[0] = 0;
        for (i = 1; i <= JPEG_HUFF_LENGTH; i++) {
            bits[i] = (td_u8)jpeg_stream_read_one_byte(par);
            count += bits[i];
        }
        len -= 1 + JPEG_HUFF_LENGTH;

        if ((count > JPEG_HUFF_CODE_SIZE) || (count > len) || (count == 0)) {
            return EXT_FAILURE;
        }

        for (i = 0; i < count; i++) {
            huffval[i] = (td_u8)jpeg_stream_read_one_byte(par);
        }
        len -= count;

        if (((td_u32)hor_table_index & 0x10) != 0) {
            hor_table_index -= 0x10;
            if ((hor_table_index < 0) || (hor_table_index >= JPEG_HUFF_TABLES)) {
                return EXT_FAILURE;
            }
            htblptr = &par->ac_huff_table[hor_table_index];
            par->ac_huff_table[hor_table_index].is_has_huff_table = TD_TRUE;
        } else {
            if ((hor_table_index < 0) || (hor_table_index >= JPEG_HUFF_TABLES)) {
                return EXT_FAILURE;
            }
            htblptr = &par->dc_huff_table[hor_table_index];
            par->dc_huff_table[hor_table_index].is_has_huff_table = TD_TRUE;
        }
        err_ret = memcpy_s(htblptr->bits, sizeof(htblptr->bits), bits, sizeof(bits));
        if (err_ret != EOK) {
            return EXT_FAILURE;
        }
        err_ret = memcpy_s(htblptr->huff_value, sizeof(htblptr->huff_value), huffval, sizeof(huffval));
        if (err_ret != EOK) {
            return EXT_FAILURE;
        }
    }
    return EXT_SUCCESS;
}

STATIC td_s32 jpeg_parse_dqt(jpeg_decompress_par_ptr par)
{
    td_u32 i, len, code, precision;
    my_jpeg_quant_table *quantptr = TD_NULL;
    td_s32 zorder[JPEG_DCT_SIZE2 + JPEG_DCT_LENGTH] = {
        0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18, 11, 4,  5,  12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,
        6,  7,  14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31,
        39, 46, 53, 60, 61, 54, 47, 55, 62, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63
    };

    len = (td_u32)jpeg_stream_read_two_byte(par);
    len -= 2; /* 2: index */

    while (len > 0) {
        code = (td_u32)jpeg_stream_read_one_byte(par);
        precision = code >> 4; /* 4: shift val */
        code &= 0x0F;
        if (code >= JPEG_QUANT_TABLES) {
            return EXT_FAILURE;
        }

        par->quant_table[code].is_has_quant_table = TD_TRUE;
        quantptr = &par->quant_table[code];

        for (i = 0; i < JPEG_DCT_SIZE2; i++) {
            if (precision != 0) {
                code = (td_u32)jpeg_stream_read_two_byte(par);
            } else {
                code = (td_u32)jpeg_stream_read_one_byte(par);
            }
            quantptr->quant_value[zorder[i]] = (td_u16)code;
        }

        len -= JPEG_DCT_SIZE2 + 1;
        if (precision != 0) {
            len -= JPEG_DCT_SIZE2;
        }
    }

    if (len != 0) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 jpeg_parse_dri(jpeg_decompress_par_ptr par)
{
    td_u32 len, code;

    len = (td_u32)jpeg_stream_read_two_byte(par);
    if (len != 4) { /* 4: length */
        return EXT_FAILURE;
    }

    code = (td_u32)jpeg_stream_read_two_byte(par);
    par->reg_mgr.restart_interval = code;

    return EXT_SUCCESS;
}

STATIC td_s32 jpeg_parse_com(jpeg_decompress_par_ptr par)
{
    td_u32 len;
    len = (td_u32)jpeg_stream_read_two_byte(par);
    len = (len >= JPEG_2BYTES) ? (len - JPEG_2BYTES) : 0;
    jpeg_stream_skip_bytes(par, len);
    return EXT_SUCCESS;
}

STATIC td_s32 jpeg_parse_decompress_info(jpeg_decompress_par_ptr par)
{
    td_s32 ret;

    ret = jpeg_table_decompress(par);
    if (ret != EXT_SUCCESS) {
        gpu_err("call jpeg_table_decompress failure");
        return EXT_FAILURE;
    }

    ret = jpeg_parse_calc_scale(par);
    if (ret != EXT_SUCCESS) {
        gpu_err("call jpeg_parse_calc_scale failure");
        return EXT_FAILURE;
    }

    ret = jpeg_parse_calc_input_color_space(par);
    if (ret != EXT_SUCCESS) {
        gpu_err("call jpeg_parse_calc_input_color_space failure");
        return EXT_FAILURE;
    }

    jpeg_parse_calc_output_color_space(par);

    jpeg_parse_calc_output_resolution(par);

    return EXT_SUCCESS;
}

STATIC td_s32 jpeg_parse_calc_scale(jpeg_decompress_par_ptr par)
{
    if (par->scale == MY_JPEG_SCALE_1) {
        par->reg_mgr.scale = 0;
    } else if (par->scale == MY_JPEG_SCALE_2) {
        par->reg_mgr.scale = 1;
    } else if (par->scale == MY_JPEG_SCALE_4) {
        par->reg_mgr.scale = 2; /* 2: sacle val */
    } else if (par->scale == MY_JPEG_SCALE_8) {
        par->reg_mgr.scale = 3; /* 3: sacle val */
    } else {
        gpu_err("scale is not support");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

STATIC td_s32 jpeg_parse_calc_input_color_space(jpeg_decompress_par_ptr par)
{
    if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV400) {
        par->reg_mgr.jpeg_color_space = 0; /* 0 for set reg yuv400 */
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV420) {
        par->reg_mgr.jpeg_color_space = 3; /* 3 for set reg yuv420 */
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV422_21) {
        par->reg_mgr.jpeg_color_space = 4; /* 4 for set reg yuv422_2 */
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV422_12) {
        par->reg_mgr.jpeg_color_space = 5; /* 5 for set reg yuv422_12 */
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV444) {
        par->reg_mgr.jpeg_color_space = 6; /* 6 for set reg yuv444 */
    } else {
        gpu_err("jpeg color space is not support");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

STATIC td_void jpeg_parse_calc_output_color_space(jpeg_decompress_par_ptr par)
{
    if ((par->output_color_space != MY_JPEG_COLOR_SPACE_ARGB8888) &&
        (par->output_color_space != MY_JPEG_COLOR_SPACE_RGB888)) {
        par->output_color_space = par->image_color_space;
    }

    if ((par->output_yuv420sp == TD_TRUE) && (par->image_color_space != MY_JPEG_COLOR_SPACE_YUV400)) {
        par->output_color_space = MY_JPEG_COLOR_SPACE_YUV420;
        par->reg_mgr.output_color_space = 1;
    }

    par->reg_mgr.uv_order = (par->out_uv_order == TD_TRUE) ? (0) : (1);

    return;
}

STATIC td_void jpeg_parse_calc_output_resolution(jpeg_decompress_par_ptr par)
{
    jpeg_parse_calc_output_wh(par);
    jpeg_parse_calc_output_stride(par);
    jpeg_parse_calc_output_mcuwh(par);
    jpeg_parse_calc_output_yuvwh(par);
    return;
}

STATIC td_void jpeg_parse_calc_output_wh(jpeg_decompress_par_ptr par)
{
    if ((par->scale >= MY_JPEG_SCALE_1) && (par->scale <= MY_JPEG_SCALE_8)) {
        par->output_width  = (td_u32)((par->image_width  + (td_u32)par->scale - 1L) / (td_u32)par->scale);
        par->output_height = (td_u32)((par->image_height + (td_u32)par->scale - 1L) / (td_u32)par->scale);
    }
}

STATIC td_void jpeg_parse_calc_output_mcuwh(jpeg_decompress_par_ptr par)
{
    td_u8 y_h_fac = (td_u8)((par->reg_mgr.y_fac >> 4) & 0xf); /* shift 4 is h */
    td_u8 y_v_fac = (td_u8)(par->reg_mgr.y_fac & 0xf); /* no shift is v */

    if (y_h_fac == 1) {
        par->reg_mgr.image_mcu_width = (par->image_width + 8 - 1) >> 3;  /* 8 align index, 3 shif val */
    } else {
        par->reg_mgr.image_mcu_width = (par->image_width + 16 - 1) >> 4; /* 16 align index, 4 shif val */
    }

    if (y_v_fac == 1) {
        par->reg_mgr.image_mcu_height = (par->image_height + 8 - 1) >> 3; /* 8 align index, 3 shif val */
        par->reg_mgr.y_mcu_height = (par->output_height + 8 - 1) & (~(8 - 1)); /* 8 align index */
    } else {
        par->reg_mgr.image_mcu_height = (par->image_height + 16 - 1) >> 4;  /* 16 align index, 4 shif val */
        par->reg_mgr.y_mcu_height = (par->output_height + 16 - 1) & (~(16 - 1)); /* 16 align index */
    }
}

STATIC td_void jpeg_parse_calc_output_yuvwh(jpeg_decompress_par_ptr par)
{
    par->reg_mgr.uv_mcu_height = 0;
    par->output.width[0]  = par->output_width;
    par->output.height[0] = par->output_height;
    if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV400) {
        par->output.width[0]  = (par->output.width[0] >> 1) << 1;
        par->output.height[0] = (par->output.height[0] >> 1) << 1;
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV420) {
        par->output.width[0]  = (par->output.width[0] >> 1) << 1;
        par->output.height[0] = (par->output.height[0] >> 1) << 1;
        par->output.width[1]  = par->output.width[0] >> 1;
        par->output.height[1] = par->output.height[0] >> 1;
        par->reg_mgr.uv_mcu_height = par->reg_mgr.y_mcu_height >> 1;
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV422_21) {
        par->output.width[0]  = (par->output.width[0] >> 1) << 1;
        par->output.width[1]  = par->output.width[0] >> 1;
        par->output.height[1] = par->output.height[0];
        par->reg_mgr.uv_mcu_height = par->reg_mgr.y_mcu_height;
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV422_12) {
        par->output.height[0] = (par->output.height[0] >> 1) << 1;
        par->output.width[1]  = par->output.width[0];
        par->output.height[1] = par->output.height[0] >> 1;
        par->reg_mgr.uv_mcu_height = par->reg_mgr.y_mcu_height >> 1;
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV444) {
        par->output.width[1]  = par->output.width[0];
        par->output.height[1] = par->output.height[0];
        par->reg_mgr.uv_mcu_height = par->reg_mgr.y_mcu_height;
    } else {
        /* nothing to do */
        gpu_err("output size is not support");
    }

    if ((par->output_yuv420sp == TD_TRUE) && (par->image_color_space != MY_JPEG_COLOR_SPACE_YUV400)) {
        par->output.width[0]  = par->output.width[0] - (par->output.width[0] % 2);   /* 2 is tolower even number */
        par->output.height[0] = par->output.height[0] - (par->output.height[0] % 2); /* 2 is tolower even number */
        par->output.width[1]  = par->output.width[0] >> 1;
        par->output.height[1] = par->output.height[0] >> 1;
        par->reg_mgr.uv_mcu_height = par->reg_mgr.y_mcu_height >> 1;
    }

    if (par->output_width == 1) {
        par->output.width[0]  = par->output_width;
        par->output.width[1]  = par->output.width[0];
    }

    if (par->output_height == 1) {
        par->output.height[0] = par->output_height;
        par->output.height[1] = par->output.height[0];
    }

    par->reg_mgr.y_mcu_height = par->output.height[0];
    par->output.size[0] = par->reg_mgr.y_mcu_height  * par->output.stride[0];
    par->output.size[1] = par->reg_mgr.uv_mcu_height * par->output.stride[1];
    return;
}

STATIC td_void jpeg_parse_calc_output_stride(jpeg_decompress_par_ptr par)
{
    /* y stride 128 bytes align, for uv_addr need 128 algin */
    par->output.stride[0] = (par->output_width + 128 - 1) & (~(128 - 1));

    if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV400) {
        par->output.stride[1] = 0;
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV420) {
        par->output.stride[1] = par->output.stride[0];
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV422_21) {
        par->output.stride[1] = par->output.stride[0];
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV422_12) {
        par->output.stride[1] = (par->output_yuv420sp == TD_TRUE) ?
                                ((par->output.stride[0] << 1) >> 1) : (par->output.stride[0] << 1);
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV444) {
        par->output.stride[1] = (par->output_yuv420sp == TD_TRUE) ?
                                ((par->output.stride[0] << 1) >> 1) : (par->output.stride[0] << 1);
    } else {
        /* nothing to do */
    }

    if (par->output_color_space == MY_JPEG_COLOR_SPACE_RGB888) {
        par->output_stride = (par->output_width * 3 + 16 - 1) & (~(16 - 1)); /* 2 xrgb need 3 and 16 bytes align */
    } else if (par->output_color_space == MY_JPEG_COLOR_SPACE_ARGB8888) {
        par->output_stride = (par->output_width * 4 + 16 - 1) & (~(16 - 1)); /* 2 xrgb need 4 and 16 bytes align */
    } else {
        /* nothing to do */
    }
}
