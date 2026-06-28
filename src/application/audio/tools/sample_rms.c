/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: RMS tool
 * Author: audio
 * Create: 2026-01-28
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "thread_os.h"
#include "securec.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"
#include "sample_audio_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define RMS_16BIT_LIMIT (20 * log10(32768)) /* 32768为s16的最大值 */
#define rms_to_db(squares_mean) (10 * log10(squares_mean) - RMS_16BIT_LIMIT) /* 将平均平方和转换为dB */
#define FILE_SIZE_LIMIT (0x8000000) /* file最大为128MB */
#define STREAM_SIZE_LIMIT (0x800) /* 一次统计2k字节对应码流的平方和 */

static td_s32 sample_rms_check_file_size(FILE *h_file)
{
    td_u64 len;

    if (h_file == TD_NULL) {
        sap_err_log_info("file is null!\n");
        return AUDIO_FAILURE;
    }

    sap_fseek(h_file, 0, SEEK_END);
    len = (td_u64)sap_ftell(h_file);
    if (len > FILE_SIZE_LIMIT) {
        audio_log1(AUDIO_LOG_LEVEL_ERROR, "the len is %u B, over than 128MB", len);
        return AUDIO_FAILURE;
    }

    if (sap_fseek(h_file, 0, SEEK_SET) != 0) {
        sap_err_log_info("fseek fail!\n");
        return AUDIO_FAILURE;
    }

    return AUDIO_SUCCESS;
}

static td_double sample_rms_calculate_sum(const td_u8 *from, const td_u32 size)
{
    td_u32 i;
    td_s16 *in_buf = (td_s16 *)from;
    const td_u32 samples_num = (size >> 1); /* pcm录音为16bit一个采样点 */
    td_double square_sum = 0;

    for (i = 0; i < samples_num; i++) {
        square_sum += (td_s32)(in_buf[i] * in_buf[i]);
    }

    return square_sum;
}

static td_void sample_rms_calculate(FILE *h_file)
{
    td_double squares_sum = 0;
    td_u32 samples_num = 0;
    td_float volume_avg;
    td_double squares_mean = 0;
    td_u32 len;
    uapi_stream_buf stream;

    stream.data = (td_u8 *)malloc(STREAM_SIZE_LIMIT);
    if (stream.data == TD_NULL) {
        sap_err_log_info("stream data open fail!\n");
        return;
    }
    stream.size = STREAM_SIZE_LIMIT;
    stream.pts = 0LL;
    stream.eos = TD_FALSE;
    stream.pkg_loss = TD_FALSE;

    while (1) {
        len = sap_fread(stream.data, 1, stream.size, h_file);
        if (len == 0) {
            break;
        }

        squares_sum += sample_rms_calculate_sum((const td_u8 *)stream.data, len);
        samples_num += (len >> 1);
    }

    if (samples_num == 0) {
        sap_err_log_info("the file is NULL!\n");
    } else {
        squares_mean = squares_sum / samples_num;
        volume_avg = (td_float)rms_to_db(squares_mean);
        sap_printf("avg of volume : %f dB", volume_avg);
    }

    free(stream.data);
    return;
}

static td_s32 sample_rms_entry(td_char *argv[])
{
    td_s32 ret;
    FILE *h_file = sample_audio_open_input_stream(argv[1]);
    if (h_file == TD_NULL) {
        sap_err_log_str(argv[1]);
        return AUDIO_SUCCESS;
    }
    ret = sample_rms_check_file_size(h_file);
    if (ret == AUDIO_SUCCESS) {
        sample_rms_calculate(h_file);
    }

    sap_fclose(h_file);
    return AUDIO_SUCCESS;
}

static td_void sample_rms_usage(const td_char *name)
{
    sap_printf("%s file(only pcm_16bit_1ch, and size < 128MB)\n", name);
    sap_printf("\n");
}

td_s32 sample_rms(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_rms_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return EXT_SUCCESS;
    } else {
        return sample_rms_entry(argv);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
