/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: effect sample
 * Author: audio
 * Create: 2025-11-26
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "thread_os.h"
#include "osal_list.h"
#include "securec.h"
#include "soc_uapi_sound.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"
#include "audio_aef.h"

#ifdef SAP_EFFECT_HVS_SUPPORT
#include "hvs_aef.h"
#endif
#ifdef SAP_EFFECT_SWS_SUPPORT
#include "sws_aef.h"
#endif
#ifdef SAP_SEA_SEE_1MIC_SUPPORT
#include "sea_aef.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define GUI_EFFECT_INVALID_HANDLE 0XFF
#define EFFECT_IMPORT_ARGC 6
#define EFFECT_EXPORT_ARGC 6
#define EFFECT_READ_ARGC 6
#define EFFECT_WRITE_ARGC 9
#define EFFECT_ENABLE_ARGC 4

typedef struct {
    td_handle h_adp_in;
    const td_char *in_stream;
    sample_acodec_arg adec_arg;
    td_handle h_snd;
    FILE *h_file_in;
    td_u32 in_data_addr;
    td_u32 in_data_size;

    struct osal_list_head node;
} sample_effect_play_inst;

typedef struct {
    td_char *sample; /* sample_effect */
    td_char *effect_type; /* hvs, sws, see_1mic_v2 */
    td_char *handle; /* [handle], 0 */
    td_char *action; /* read, write, import, export, on, off */
    td_char *parabin_name; /* algorithm, [parabin_name] */
    td_char *cmd_id; /* cmd id(hex) for param set/get [exact_cmd/'null'/'NULL'/optional] */
    td_char *para_byte_offset; /* [para_byte_offset] */
    td_char *para_type; /* short, unsigned_short, signed_char, unsigned_char */
    td_char *para_val; /* [para_val] */
    td_char *reserve; /* null */
} effect_arg;

typedef struct {
    td_s32 (*get_handle)(const effect_arg *arg, td_handle *handle);
    td_s32 (*import)(td_handle handle, const effect_arg *arg, td_void *param, td_u32 param_size);
    td_s32 (*export)(td_handle handle, const effect_arg *arg, td_void *param, td_u32 param_size);
    td_s32 (*read)(td_handle handle);
    td_s32 (*write)(td_handle handle, const effect_arg *arg, td_void *param, td_u32 param_size);
    td_s32 (*enable)(td_handle handle, const effect_arg *arg);
    td_void *param_data;
    td_u32 param_size;
} effect_type_info;

static OSAL_LIST_HEAD(g_sample_effect_play_list);

static sample_effect_play_inst *sample_effect_play_alloc_inst(td_void)
{
    sample_effect_play_inst *inst = (sample_effect_play_inst *)malloc(sizeof(sample_effect_play_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    (td_void)memset_s(inst, sizeof(*inst), 0, sizeof(sample_effect_play_inst));

    osal_list_add_tail(&inst->node, &g_sample_effect_play_list);
    return inst;
}

static td_void sample_effect_play_free_inst(sample_effect_play_inst *inst)
{
    if (inst == TD_NULL) {
        return;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static sample_effect_play_inst *sample_effect_play_get_inst(td_void)
{
    if (osal_list_empty(&g_sample_effect_play_list) != TD_FALSE) {
        return TD_NULL;
    }

    return osal_list_first_entry(&g_sample_effect_play_list, sample_effect_play_inst, node);
}

static td_void sample_effect_usage(td_void)
{
    sap_printf("\n");
    sap_printf("If you want to play a file\n");
    sap_printf("    usage:  sample_effect -i [InputFileName] [OPTIONS]\n");
    sap_printf("\n");
    sap_printf("        -h     show this usage message and abort\n");
    sap_printf("        -i     input file name\n");
    sap_printf("        -t     the type of play audio(aac/mp3/opus/lc3/l2hc/sbc/msbc/flac)\n");
    sap_printf("\n");
    sap_printf("\n");

    sap_printf("effect_arg compositions:\n");
    sap_printf("sample  effect_type  handle  action  parabin_name  para_byte_offset  para_type  para_val\n");
    sap_printf("sample ---> sample_effect\n");
    sap_printf("effect_type ---> hvs, sws, see_1mic_v2\n");
    sap_printf("handle ---> [handle], 0\n");
    sap_printf("action ---> read, write, import, export, on, off             sea does not support off\n");
    sap_printf("parabin_name ---> algorithm, [parabin_name]\n");
    sap_printf("para_byte_offset ---> [para_byte_offset]\n");
    sap_printf("para_type ---> short, unsigned_short, signed_char, unsigned_char\n");
    sap_printf("para_val ---> [para_val]\n");
    printf("\n");

    sap_printf("effect examples:\n");
    sap_printf("Enable HVS: sample_effect hvs 0 on\n");
    sap_printf("Disable HVS: sample_effect hvs 0 off\n");
    sap_printf("Import parameters from file to algorithm: sample_effect hvs 0 import [parabin_name]\n");
    sap_printf("Export parameters from algorithm to file: sample_effect hvs 0 export [parabin_name]\n");
    sap_printf("Read parameters from algorithm: sample_effect hvs 0 read algorithm\n");
    sap_printf("Write parameters in algorithm at byte_offset 22: sample_effect hvs 0 write algorithm 22 short 10\n");
    sap_printf("\n");
    sap_printf("\n");
}

static td_void sample_effect_close_file(sample_effect_play_inst *inst)
{
    if (inst->h_file_in != TD_NULL) {
        sap_fclose(inst->h_file_in);
        inst->h_file_in = TD_NULL;
    }
}

static td_s32 sample_effect_exit(td_void)
{
    sample_effect_play_inst *inst = sample_effect_play_get_inst();
    if (inst != TD_NULL) {
        sample_escast_close();
        sample_effect_close_file(inst);
        sample_effect_play_free_inst(inst);
    }
    return EXT_SUCCESS;
}

static td_s32 sample_effect_parse_opt(td_char opt, const td_char *opt_arg, sample_effect_play_inst *inst)
{
    if (opt == 'h') {
        sample_effect_usage();
        return EXT_FAILURE;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return EXT_FAILURE;
    }

    switch (opt) {
        case 'i':
            inst->in_stream = opt_arg;
            parse_audio_params_from_file(inst->in_stream, &inst->adec_arg.pcm_format);
            break;
        case 't': /* direct return */
            return sample_audio_get_acodec_id(opt_arg, &inst->adec_arg.acodec_id);
        default:
            sap_printf("invalid command line\n");
            sample_effect_usage();
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_effect_parse_cmd_line(td_s32 argc, td_char *argv[], sample_effect_play_inst *inst)
{
    parg_state arg_parse;
    td_s32 ret;
    td_char opt;

    if ((argv == TD_NULL) || (argc < 0x2)) {
        goto out;
    }

    parg_init(&arg_parse);
    while (1) {
        ret = parg_getopt(&arg_parse, argc, argv, "i:t:h");
        if (ret == -1) {
            break;
        }
        opt = (td_char)ret;
        if (sample_effect_parse_opt(opt, arg_parse.optarg, inst) != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
out:
    sample_effect_usage();
    return EXT_FAILURE;
}

static td_void sample_effect_inst_init(sample_effect_play_inst *inst)
{
    inst->adec_arg.pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    inst->adec_arg.pcm_format.channels = UAPI_AUDIO_CHANNEL_2;
    inst->adec_arg.pcm_format.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    inst->adec_arg.pcm_format.sample_per_frame = (inst->adec_arg).pcm_format.sample_rate / 100; /* 100 --> 10ms */
    inst->adec_arg.acodec_id = UAPI_ACODEC_ID_MAX;
    inst->adec_arg.dnlink_sea_en = TD_FALSE;
    inst->adec_arg.file = TD_NULL;

    inst->h_adp_in = 0;
    inst->h_snd = 0;
    inst->in_data_addr = 0;
    inst->in_data_size = 0;
}

static td_s32 sample_effect_open_input_file(sample_effect_play_inst *inst)
{
    inst->h_file_in = TD_NULL;
    inst->h_file_in = sample_audio_open_input_stream(inst->in_stream);
    if (inst->h_file_in == TD_NULL) {
        sap_printf("sap fopen %s failed", inst->in_stream);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_effect_entry(int argc, td_char *argv[])
{
    td_s32 ret;
    sample_effect_play_inst *inst = sample_effect_play_alloc_inst();
    if (inst == TD_NULL) {
        sap_printf("sample_effect_play_alloc_inst_failed\n");
        return EXT_FAILURE;
    }

    const uapi_snd_attr snd_attr = {
        .port_num = 1,
        .port_attr[0].out_port = SND_OUT_PORT_DEFAULT,
        .channels = SND_OUT_CHANNEL_DEF,
        .bit_depth = SND_OUT_BIT_DEPTH_DEF,
        .sample_rate = SND_OUT_SAMPLE_RATE_DEF,
    };

    sample_effect_inst_init(inst);

    ret = sample_effect_parse_cmd_line(argc, argv, inst);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_effect_open_input_file(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_effect_open_input_file, ret);
        goto out0;
    }

    sample_escast_arg escast_arg = {
        .player = &inst->h_adp_in,
        .in_arg = &inst->adec_arg,
        .snd = &inst->h_snd,
        .snd_attr = &snd_attr,
        .audio_scene = UAPI_AUDIO_SCENE_LOCAL_MUSIC,
        .in_file = inst->h_file_in,
        .in_data_addr = inst->in_data_addr,
        .in_data_size = inst->in_data_size,
    };

    ret = sample_escast_open(&escast_arg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_escast_open, ret);
        goto out1;
    }

    return EXT_SUCCESS;

out1:
    sample_effect_close_file(inst);
out0:
    sample_effect_play_free_inst(inst);
    return ret;
}

#if defined(SAP_SEA_SEE_1MIC_SUPPORT) || defined(SAP_EFFECT_HVS_SUPPORT)
static td_void short_param_print_array_1d(const char *name, const td_s16 *arr, const td_u16 rows)
{
    td_u16 i;
    for (i = 0; i < rows; i++) {
        sap_printf("%s[%u]: %hd> ", name, i, arr[i]);
    }
    sap_printf("\n");
}

static td_void signed_char_param_print_array_1d(const char *name, const td_s8 *arr, const td_u16 rows)
{
    td_u16 i;
    for (i = 0; i < rows; i++) {
        sap_printf("%s[%u]: %hhd> ", name, i, arr[i]);
    }
    sap_printf("\n");
}

static void short_param_print_array_2d(const char *name, const td_s16 *arr, td_u16 rows, td_u16 cols)
{
    td_u16 i, j;
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
            sap_printf("<%s[%u][%u]: %hd> ", name, i, j, arr[i * cols + j]);
        }
        sap_printf("\n");
    }
}
#endif

#ifdef SAP_EFFECT_HVS_SUPPORT
static void hvs_param_printf_alg_module(const audio_hvs_param_struct *param)
{
    sap_printf("\n");
    sap_printf("\n");
    sap_printf("ALG module(0-7): \n");

    sap_printf("ntm_flag: %hd| qmf_delay: %hd| total_gain: %hd \n", param->ntm_flag, param->qmf_delay,
        param->total_gain);

    short_param_print_array_1d("geq0_pre_gain", (const td_s16 *)(param->reserved),
        sizeof(param->reserved) / sizeof(param->reserved[0]));

    sap_printf("\n");
}

static void hvs_param_printf_cgn_module(const audio_hvs_param_struct *param)
{
    sap_printf("CGN module(8-11): \n");

    sap_printf("cgn_module_flag: %hd| vol_relate: %hd \n", param->cgn_module_flag, param->vol_relate);

    short_param_print_array_1d("gain", (const td_s16 *)(param->gain), sizeof(param->gain) / sizeof(param->gain[0]));

    sap_printf("\n");
}

static void hvs_param_printf_lme_module(const audio_hvs_param_struct *param)
{
    sap_printf("LME module(12-26): \n");

    sap_printf("lme_module_flag: %hd \n", param->lme_module_flag);

    short_param_print_array_1d("mce", (const td_s16 *)(param->mce), sizeof(param->mce) / sizeof(param->mce[0]));

    short_param_print_array_1d("lfe", (const td_s16 *)(param->lfe), sizeof(param->lfe) / sizeof(param->lfe[0]));

    sap_printf("\n");
}

static void hvs_param_printf_geq0_module(const audio_hvs_param_struct *param)
{
    sap_printf("GEQ0 module(27-127): \n");

    sap_printf("geq0_module_flag: %hd \n", param->geq0_module_flag);

    short_param_print_array_1d("geq0_pre_gain", (const td_s16 *)(param->geq0_pre_gain),
        sizeof(param->geq0_pre_gain) / sizeof(param->geq0_pre_gain[0]));

    short_param_print_array_1d("geq0_n", (const td_s16 *)(param->geq0_n),
        sizeof(param->geq0_n) / sizeof(param->geq0_n[0]));

    short_param_print_array_2d("geq0_type", (const td_s16 *)(param->geq0_type),
        sizeof(param->geq0_type) / sizeof(param->geq0_type[0]),
        sizeof(param->geq0_type[0]) / sizeof(param->geq0_type[0][0]));

    short_param_print_array_2d("geq0_g", (const td_s16 *)(param->geq0_g),
        sizeof(param->geq0_g) / sizeof(param->geq0_g[0]),
        sizeof(param->geq0_g[0]) / sizeof(param->geq0_g[0][0]));

    short_param_print_array_2d("geq0_q", (const td_s16 *)(param->geq0_q),
        sizeof(param->geq0_q) / sizeof(param->geq0_q[0]),
        sizeof(param->geq0_q[0]) / sizeof(param->geq0_q[0][0]));

    short_param_print_array_2d("geq0_fc", (const td_s16 *)(param->geq0_fc),
        sizeof(param->geq0_fc) / sizeof(param->geq0_fc[0]),
        sizeof(param->geq0_fc[0]) / sizeof(param->geq0_fc[0][0]));

    sap_printf("\n");
}

static void hvs_param_printf_geq1_module(const audio_hvs_param_struct *param)
{
    sap_printf("GEQ1 module(128-228): \n");

    sap_printf("geq1_module_flag: %hd \n", param->geq1_module_flag);

    short_param_print_array_1d("geq1_pre_gain", (const td_s16 *)(param->geq1_pre_gain),
        sizeof(param->geq1_pre_gain) / sizeof(param->geq1_pre_gain[0]));

    short_param_print_array_1d("geq1_n", (const td_s16 *)(param->geq1_n),
        sizeof(param->geq1_n) / sizeof(param->geq1_n[0]));

    short_param_print_array_2d("geq1_type", (const td_s16 *)(param->geq1_type),
        sizeof(param->geq1_type) / sizeof(param->geq1_type[0]),
        sizeof(param->geq1_type[0]) / sizeof(param->geq1_type[0][0]));

    short_param_print_array_2d("geq1_g", (const td_s16 *)(param->geq1_g),
        sizeof(param->geq1_g) / sizeof(param->geq1_g[0]),
        sizeof(param->geq1_g[0]) / sizeof(param->geq1_g[0][0]));

    short_param_print_array_2d("geq1_q", (const td_s16 *)(param->geq1_q),
        sizeof(param->geq1_q) / sizeof(param->geq1_q[0]),
        sizeof(param->geq1_q[0]) / sizeof(param->geq1_q[0][0]));

    short_param_print_array_2d("geq1_fc", (const td_s16 *)(param->geq1_fc),
        sizeof(param->geq1_fc) / sizeof(param->geq1_fc[0]),
        sizeof(param->geq1_fc[0]) / sizeof(param->geq1_fc[0][0]));

    sap_printf("\n");
}

static void hvs_param_printf_lsp_module(const audio_hvs_param_struct *param)
{
    sap_printf("LSP module(229-301): \n");

    sap_printf("lsp_module_flag: %hd \n", param->lsp_module_flag);

    short_param_print_array_1d("sub_module_flag", (const td_s16 *)(param->sub_module_flag),
        sizeof(param->sub_module_flag) / sizeof(param->sub_module_flag[0]));

    short_param_print_array_1d("makeup_gain", (const td_s16 *)(param->makeup_gain),
        sizeof(param->makeup_gain) / sizeof(param->makeup_gain[0]));

    short_param_print_array_1d("attack", (const td_s16 *)(param->attack),
        sizeof(param->attack) / sizeof(param->attack[0]));

    short_param_print_array_1d("release", (const td_s16 *)(param->release),
        sizeof(param->release) / sizeof(param->release[0]));

    short_param_print_array_1d("thr", (const td_s16 *)(param->thr),
        sizeof(param->thr) / sizeof(param->thr[0]));

    short_param_print_array_1d("ratio", (const td_s16 *)(param->ratio),
        sizeof(param->ratio) / sizeof(param->ratio[0]));

    short_param_print_array_1d("softknee", (const td_s16 *)(param->softknee),
        sizeof(param->softknee) / sizeof(param->softknee[0]));

    short_param_print_array_1d("p_sm_coeff", (const td_s16 *)(param->p_sm_coeff),
        sizeof(param->p_sm_coeff) / sizeof(param->p_sm_coeff[0]));

    short_param_print_array_1d("pmax", (const td_s16 *)(param->pmax),
        sizeof(param->pmax) / sizeof(param->pmax[0]));

    short_param_print_array_2d("x_u2x_params", (const td_s16 *)(param->x_u2x_params),
        sizeof(param->x_u2x_params) / sizeof(param->x_u2x_params[0]),
        sizeof(param->x_u2x_params[0]) / sizeof(param->x_u2x_params[0][0]));

    short_param_print_array_1d("x_delay", (const td_s16 *)(param->x_delay),
        sizeof(param->x_delay) / sizeof(param->x_delay[0]));

    sap_printf("\n");
}

static void hvs_param_printf(const audio_hvs_param_struct *param)
{
    hvs_param_printf_alg_module(param);
    hvs_param_printf_cgn_module(param);
    hvs_param_printf_lme_module(param);
    hvs_param_printf_geq0_module(param);
    hvs_param_printf_geq1_module(param);
    hvs_param_printf_lsp_module(param);
}

static td_s32 hvs_read_algorithm(td_handle handle)
{
    audio_hvs_param_struct parameter;
    td_s32 ret;

    ret = uapi_snd_get_aef_param(handle, SND_OUT_PORT_DEFAULT, AEF_CMD_ALG_PARAM, &parameter,
        sizeof(audio_hvs_param_struct));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_aef_param, ret);
        return EXT_FAILURE;
    }

    hvs_param_printf(&parameter);

    return EXT_SUCCESS;
}
#endif

#ifdef SAP_EFFECT_SWS_SUPPORT
static void sws_param_printf(const audio_sws_mobile_para *param)
{
    sap_printf("sws_param: \n");

    sap_printf("module: %hd| x2b_surround: %hd| x2b_sound_width: %hd| eq_pre_gain_lr: %hd| eq_bands_lr: %hd\n",
        param->module, param->x2b_surround, param->x2b_sound_width, param->eq_pre_gain_lr, param->eq_bands_lr);

    short_param_print_array_1d("eq_type_lr", (const td_s16 *)(param->eq_type_lr),
        sizeof(param->eq_type_lr) / sizeof(param->eq_type_lr[0]));

    short_param_print_array_1d("eq_gain_lr", (const td_s16 *)(param->eq_gain_lr),
        sizeof(param->eq_gain_lr) / sizeof(param->eq_gain_lr[0]));

    short_param_print_array_2d("eq_q", (const td_s16 *)(param->eq_q),
        sizeof(param->eq_q) / sizeof(param->eq_q[0]),
        sizeof(param->eq_q[0]) / sizeof(param->eq_q[0][0]));

    short_param_print_array_2d("eq_fc", (const td_s16 *)(param->eq_fc),
        sizeof(param->eq_fc) / sizeof(param->eq_fc[0]),
        sizeof(param->eq_fc[0]) / sizeof(param->eq_fc[0][0]));

    sap_printf("drc_gain: %hd| drc_paral_glr: %hd| drc_looka_lr: %hd| lmt_th: %hd\n",
        param->drc_gain, param->drc_paral_glr, param->drc_looka_lr, param->lmt_th);

    sap_printf("\n");
}

static td_s32 sws_read_algorithm(td_handle handle)
{
    audio_sws_mobile_para parameter;
    td_s32 ret;

    ret = uapi_snd_get_aef_param(handle, SND_OUT_PORT_DEFAULT, AEF_CMD_ALG_PARAM, &parameter,
        sizeof(audio_sws_mobile_para));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_aef_param, ret);
        return EXT_FAILURE;
    }

    sws_param_printf(&parameter);

    return EXT_SUCCESS;
}
#endif

#ifdef SAP_SEA_SEE_1MIC_SUPPORT
static void see_1mic_v2_param_printf(const audio_vqe_param_struct *param)
{
    sap_printf("sea_param: \n");

    sap_printf("aiwnr_enable: %hhu| aec_enable: %hhu| dmnr_enable: %hhu\n",
        param->aiwnr_enable, param->aec_enable, param->dmnr_enable);

    sap_printf("agc_enable: %hhu| fmp_enable: %hhu| eq_enable: %hhu\n",
        param->agc_enable, param->fmp_enable, param->eq_enable);

    sap_printf("x_change_gain: %hd| us_zoom_factor1: %hu| post_filter2: %hhu| nlms_block_num1: %hhu\n",
        param->x_change_gain, param->us_zoom_factor1, param->post_filter2, param->nlms_block_num1);

    sap_printf("high_proc_flag: %hhu| sd_zoom_factor: %hd| min_gain_limit: %hd| post_snr_smooth_factor: %hu\n",
        param->high_proc_flag, param->sd_zoom_factor, param->min_gain_limit, param->post_snr_smooth_factor);

    sap_printf("spp_smooth_factor: %hu| noise_pwr_smooth_factor: %hu\n",
        param->spp_smooth_factor, param->noise_pwr_smooth_factor);

    sap_printf("target_level: %hd| max_gain: %hd| min_gain: %hd| up_gradient_ratio: %hd\n",
        param->target_level, param->max_gain, param->min_gain, param->up_gradient_ratio);

    signed_char_param_print_array_1d("eq_band_gain", (const td_s8 *)(param->eq_band_gain),
        sizeof(param->eq_band_gain) / sizeof(param->eq_band_gain[0]));

    sap_printf("\n");
}

static td_s32 see_1mic_v2_read_algorithm(td_handle handle)
{
    audio_vqe_param_struct parameter;
    td_s32 ret;

    ret = uapi_sea_get_param(handle, UAPI_SEA_AFE_SEE, (td_u8 *)&parameter, sizeof(audio_vqe_param_struct));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_get_param, ret);
        return ret;
    }

    see_1mic_v2_param_printf(&parameter);

    return EXT_SUCCESS;
}
#endif

static td_s32 get_enable_flag_by_action_param(const td_char *action_str, td_bool *en_flag)
{
    if (strcmp(action_str, "on") == 0) {
        *en_flag = TD_TRUE;
    } else if (strcmp(action_str, "off") == 0) {
        *en_flag = TD_FALSE;
    } else {
        sap_err_log_str(action_str);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_effect_aef_enable_entry(td_handle handle, const effect_arg *arg)
{
    td_s32 ret;
    td_bool enable;

    ret = get_enable_flag_by_action_param(arg->action, &enable);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(get_enable_flag_by_action_param, ret);
        return ret;
    }

    ret = uapi_snd_set_aef_bypass(handle, UAPI_SND_OUT_PORT_ALL, !enable); /* bypass = !enable */
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_bypass, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_effect_aef_pre_enable_entry(td_handle handle, const effect_arg *arg)
{
    td_s32 ret;
    td_bool enable;

    ret = get_enable_flag_by_action_param(arg->action, &enable);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(get_enable_flag_by_action_param, ret);
        return ret;
    }

    ret = uapi_snd_set_port_aef_enable(handle, SND_OUT_PORT_DEFAULT, UAPI_AEF_TYPE_AEF_PRE, enable);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_port_aef_enable, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_effect_sea_enable_entry(td_handle handle, const effect_arg *arg)
{
    td_s32 ret;

    if (strcmp(arg->action, "on") == 0) {
        ret = uapi_sea_start(handle);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_start, ret);
            return EXT_FAILURE;
        }
    } else if (strcmp(arg->action, "off") == 0) {
        ret = uapi_sea_stop(handle);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_stop, ret);
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}

static td_s32 sample_effect_aef_get_handle(const effect_arg *arg, td_handle *handle)
{
    if (strcmp(arg->handle, "0") == 0) {
        *handle = sample_gui_aef_get_handle(GUI_AEF_AO);
        if (*handle == GUI_EFFECT_INVALID_HANDLE) {
            sap_err_log_info("invalid handle");
            return EXT_FAILURE;
        }
        sap_printf("\r\nget handle: 0x%08X\r\n", *handle);
    } else {
        *handle = strtoul(arg->handle, TD_NULL, 16); /* 16: hexadecimal */
    }

    return EXT_SUCCESS;
}

static td_s32 sample_effect_sea_get_handle(const effect_arg *arg, td_handle *handle)
{
    if (strcmp(arg->handle, "0") == 0) {
        *handle = sample_gui_aef_get_handle(GUI_AEF_SEA);
        if (*handle == GUI_EFFECT_INVALID_HANDLE) {
            sap_err_log_info("invalid handle");
            return EXT_FAILURE;
        }
        sap_printf("get handle: 0x%08X\n", *handle);
    } else {
        *handle = strtoul(arg->handle, TD_NULL, 16); /* 16: hexadecimal */
    }

    return EXT_SUCCESS;
}

/* check str is null ptr or 'null'/'NULL' str */
#define str_null_check(str) ((str) == TD_NULL || strcmp((str), "null") == 0 || strcmp((str), "NULL") == 0)

static td_u32 update_cmd_type_by_cmd_arg(const td_char *cmd_str)
{
    td_u32 cmd_id = UAPI_SEA_AFE_SEE;

    if (str_null_check(cmd_str) != TD_TRUE) {
        cmd_id = (td_u32)strtol(cmd_str, TD_NULL, 0);
    }
    sap_alert_log_u32(cmd_id);
    return cmd_id;
}

static td_s32 sample_effect_aef_import_from_file(td_handle handle, const effect_arg *arg, td_void *param,
    td_u32 param_size)
{
    td_s32 ret;
    td_u32 file_size;
    td_u32 cmd = update_cmd_type_by_cmd_arg(arg->cmd_id);
    FILE *file = TD_NULL;

    file = sample_audio_open_input_stream(arg->parabin_name);
    if (file == TD_NULL) {
        sap_printf("open %s failed\n", arg->parabin_name);
        return EXT_FAILURE;
    }

    sap_fseek(file, 0, SEEK_END);
    file_size = (td_u32)sap_ftell(file);
    sap_rewind(file);

    param_size = sap_min(file_size, param_size);
    sap_alert_log_u32(file_size);
    sap_alert_log_u32(param_size);

    file_size = sap_fread(param, param_size, 1, file);
    if (file_size != 1) {
        sap_printf("read %s failed\n", arg->parabin_name);
        ret = EXT_FAILURE;
        goto out;
    }

    ret = uapi_snd_set_aef_param(handle, SND_OUT_PORT_DEFAULT, cmd, param, param_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_param, ret);
        goto out;
    }

    ret = EXT_SUCCESS;
out:
    sap_fclose(file);
    return ret;
}

static td_s32 sample_effect_sea_import_from_file(td_handle handle, const effect_arg *arg, td_void *param,
    td_u32 param_size)
{
    td_s32 ret;
    td_u32 file_size;
    td_u32 cmd = update_cmd_type_by_cmd_arg(arg->cmd_id);
    FILE *file = TD_NULL;

    file = sample_audio_open_input_stream(arg->parabin_name);
    if (file == TD_NULL) {
        sap_printf("open %s failed\n", arg->parabin_name);
        return EXT_FAILURE;
    }

    sap_fseek(file, 0, SEEK_END);
    file_size = (td_u32)sap_ftell(file);
    sap_rewind(file);

    param_size = sap_min(file_size, param_size);

    file_size = sap_fread(param, param_size, 1, file);
    if (file_size != 1) {
        sap_printf("read %s failed\n", arg->parabin_name);
        ret = EXT_FAILURE;
        goto out;
    }

    ret = uapi_sea_set_param(handle, cmd, param, param_size);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_set_param, ret);
            goto out;
    }

    ret = EXT_SUCCESS;
out:
    sap_fclose(file);
    return ret;
}

static td_s32 sample_effect_aef_export_to_file(td_handle handle, const effect_arg *arg, td_void *param,
    td_u32 param_size)
{
    td_s32 ret;
    td_u32 write;
    td_u32 cmd = update_cmd_type_by_cmd_arg(arg->cmd_id);
    FILE *file = TD_NULL;

    ret = uapi_snd_get_aef_param(handle, SND_OUT_PORT_DEFAULT, cmd, param, param_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_aef_param, ret);
        return ret;
    }

    file = sample_audio_open_output_stream(arg->parabin_name);
    if (file == TD_NULL) {
        sap_printf("open %s failed\n", arg->parabin_name);
        return EXT_FAILURE;
    }

    write = sap_fwrite(param, param_size, 1, file);
    if (write != 1) {
        sap_printf("write %s failed\n", arg->parabin_name);
        ret = EXT_FAILURE;
        goto out;
    }

    ret = EXT_SUCCESS;
out:
    sap_fclose(file);
    return ret;
}

static td_s32 sample_effect_sea_export_to_file(td_handle handle, const effect_arg *arg, td_void *param,
    td_u32 param_size)
{
    td_s32 ret;
    td_u32 write;
    td_u32 cmd = update_cmd_type_by_cmd_arg(arg->cmd_id);
    FILE *file = TD_NULL;

    ret = uapi_sea_get_param(handle, cmd, param, param_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_get_param, ret);
        return ret;
    }

    file = sample_audio_open_output_stream(arg->parabin_name);
    if (file == TD_NULL) {
        sap_printf("open %s failed\n", arg->parabin_name);
        return EXT_FAILURE;
    }

    write = sap_fwrite(param, param_size, 1, file);
    if (write != 1) {
        sap_printf("write %s failed\n", arg->parabin_name);
        ret = EXT_FAILURE;
        goto out;
    }

    ret = EXT_SUCCESS;
out:
    sap_fclose(file);
    return ret;
}

static td_s32 change_para_by_cmd_arg(const effect_arg *arg, td_void *param, td_u32 param_size)
{
    td_u8 *para_addr = TD_NULL;
    td_u32 para_val = (td_u32)strtol(arg->para_val, TD_NULL, 0);
    td_u32 byte_offset = (td_u32)strtol(arg->para_byte_offset, TD_NULL, 0);
    if (byte_offset > param_size) {
        sap_err_log_u32(param_size);
        sap_err_log_u32(byte_offset);
        return EXT_FAILURE;
    }
    para_addr = (td_u8 *)param + byte_offset;

    if (strcmp(arg->para_type, "s16") == 0) {
        *(td_s16 *)para_addr = (td_s16)para_val;
    } else if (strcmp(arg->para_type, "u8") == 0) {
        *(td_u8 *)para_addr = (td_u8)para_val;
    } else if (strcmp(arg->para_type, "u16") == 0) {
        *(td_u16 *)para_addr = (td_u16)para_val;
    } else if (strcmp(arg->para_type, "s8") == 0) {
        *(td_s8 *)para_addr = (td_s8)para_val;
    } else {
        sap_err_log_str(arg->para_type);
        return EXT_FAILURE;
    }

    sap_printf("Set the addr offset[%u] to val[%u]\n", byte_offset, para_val);

    return EXT_SUCCESS;
}

static td_s32 sample_effect_aef_write_algorithm(td_handle handle, const effect_arg *arg, td_void *param, td_u32 param_size)
{
    td_s32 ret;
    td_u32 cmd = update_cmd_type_by_cmd_arg(arg->cmd_id);

    ret = uapi_snd_get_aef_param(handle, SND_OUT_PORT_DEFAULT, cmd, param, param_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_aef_param, ret);
        return ret;
    }

    ret = change_para_by_cmd_arg(arg, param, param_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(change_para_by_cmd_arg, ret);
        return ret;
    }

    ret = uapi_snd_set_aef_param(handle, SND_OUT_PORT_DEFAULT, cmd, param, param_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_param, ret);
    }

    return EXT_SUCCESS;
}

static td_s32 sample_effect_sea_write_algorithm(td_handle handle, const effect_arg *arg, td_void *param, td_u32 param_size)
{
    td_s32 ret;
    td_u32 cmd = update_cmd_type_by_cmd_arg(arg->cmd_id);

    ret = uapi_sea_get_param(handle, cmd, param, param_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_get_param, ret);
        return ret;
    }

    ret = change_para_by_cmd_arg(arg, param, param_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(change_para_by_cmd_arg, ret);
        return ret;
    }

    ret = uapi_sea_set_param(handle, cmd, param, param_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_set_param, ret);
    }

    return EXT_SUCCESS;
}

#define ALG_PARAM_MAX_SIZE 5120
static effect_type_info aef_info = {
    .get_handle = sample_effect_aef_get_handle,
    .import = sample_effect_aef_import_from_file,
    .export = sample_effect_aef_export_to_file,
    .read = TD_NULL,
    .write = sample_effect_aef_write_algorithm,
    .enable = sample_effect_aef_enable_entry,
    .param_data = TD_NULL,
    .param_size = ALG_PARAM_MAX_SIZE,
};

static effect_type_info aef_pre_info = {
    .get_handle = sample_effect_aef_get_handle,
    .import = sample_effect_aef_import_from_file,
    .export = sample_effect_aef_export_to_file,
    .read = TD_NULL,
    .write = sample_effect_aef_write_algorithm,
    .enable = sample_effect_aef_pre_enable_entry,
    .param_data = TD_NULL,
    .param_size = ALG_PARAM_MAX_SIZE,
};

static effect_type_info sea_info = {
    .get_handle = sample_effect_sea_get_handle,
    .import = sample_effect_sea_import_from_file,
    .export = sample_effect_sea_export_to_file,
    .read = TD_NULL,
    .write = sample_effect_sea_write_algorithm,
    .enable = sample_effect_sea_enable_entry,
    .param_data = TD_NULL,
    .param_size = ALG_PARAM_MAX_SIZE,
};

#ifdef SAP_EFFECT_HVS_SUPPORT
static audio_hvs_param_struct g_hvs_para = {0};

static effect_type_info hvs_info = {
    .get_handle = sample_effect_aef_get_handle,
    .import = sample_effect_aef_import_from_file,
    .export = sample_effect_aef_export_to_file,
    .read = hvs_read_algorithm,
    .write = sample_effect_aef_write_algorithm,
    .enable = sample_effect_aef_enable_entry,
    .param_data = &g_hvs_para,
    .param_size = sizeof(audio_hvs_param_struct),
};
#endif

#ifdef SAP_EFFECT_SWS_SUPPORT
static audio_sws_mobile_para g_sws_para = {0};

static effect_type_info sws_info = {
    .get_handle = sample_effect_aef_get_handle,
    .import = sample_effect_aef_import_from_file,
    .export = sample_effect_aef_export_to_file,
    .read = sws_read_algorithm,
    .write = sample_effect_aef_write_algorithm,
    .enable = sample_effect_aef_enable_entry,
    .param_data = &g_sws_para,
    .param_size = sizeof(audio_sws_mobile_para),
};
#endif

#ifdef SAP_SEA_SEE_1MIC_SUPPORT
static audio_vqe_param_struct g_see_1mic_v2_para = {0};

static effect_type_info see_1mic_v2_info = {
    .get_handle = sample_effect_sea_get_handle,
    .import = sample_effect_sea_import_from_file,
    .export = sample_effect_sea_export_to_file,
    .read = see_1mic_v2_read_algorithm,
    .write = sample_effect_sea_write_algorithm,
    .enable = sample_effect_sea_enable_entry,
    .param_data = &g_see_1mic_v2_para,
    .param_size = sizeof(audio_vqe_param_struct),
};
#endif

static effect_type_info *get_effect_info_by_type(const td_char *effect_type_str)
{
    if (strcmp(effect_type_str, "aef") == 0) {
        return &aef_info;
    } else if (strcmp(effect_type_str, "aef_pre") == 0) {
        return &aef_pre_info;
    } else if (strcmp(effect_type_str, "sea") == 0) {
        return &sea_info;
    } else if (strcmp(effect_type_str, "hvs") == 0) {
#ifdef SAP_EFFECT_HVS_SUPPORT
        return &hvs_info;
#endif
    } else if (strcmp(effect_type_str, "sws") == 0) {
#ifdef SAP_EFFECT_SWS_SUPPORT
        return &sws_info;
#endif
    } else if (strcmp(effect_type_str, "see_1mic_v2") == 0) {
#ifdef SAP_SEA_SEE_1MIC_SUPPORT
        return &see_1mic_v2_info;
#endif
    }
    return TD_NULL;
}

typedef enum {
    ACTION_IMPORT = 0,
    ACTION_EXPORT,
    ACTION_READ,
    ACTION_WRITE,
    ACTION_ON,
    ACTION_OFF,
    ACTION_MAX,
} action_type_e;

static action_type_e get_action_type_by_action_str(const td_char *action_str)
{
    const td_char *all_action_str[] = {
        "import", /* ACTION_IMPORT */
        "export", /* ACTION_EXPORT */
        "read", /* ACTION_READ */
        "write", /* ACTION_WRITE */
        "on", /* ACTION_ON */
        "off", /* ACTION_OFF */
    }; /* Arrange the corresponding strings strictly according to the enum order */

    for (td_u32 i = 0; i < sizeof(all_action_str) / sizeof(all_action_str[0]); ++i) {
        if (strcasecmp(all_action_str[i], action_str) == 0) {
            return (action_type_e)i;
        }
    }

    return ACTION_MAX;
}

static td_s32 action_execute(effect_type_info *info, const effect_arg *arg, td_u32 argc, td_void *data, td_u32 size)
{
    td_handle handle;
    action_type_e action_typ;

    if (info->get_handle(arg, &handle) != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    action_typ = get_action_type_by_action_str(arg->action);
    switch (action_typ) {
        case ACTION_IMPORT:
            if (argc >= EFFECT_IMPORT_ARGC && info->import != TD_NULL) {
                return info->import(handle, arg, data, size);
            }
            break;
        case ACTION_EXPORT:
            if (argc >= EFFECT_EXPORT_ARGC && info->export != TD_NULL) {
                return info->export(handle, arg, data, size);
            }
            break;
        case ACTION_READ:
            if (argc >= EFFECT_READ_ARGC && info->read != TD_NULL) {
                return info->read(handle);
            }
            break;
        case ACTION_WRITE:
            if (argc >= EFFECT_WRITE_ARGC && info->write != TD_NULL) {
                return info->write(handle, arg, data, size);
            }
            break;
        case ACTION_ON:
        case ACTION_OFF:
            if (argc >= EFFECT_ENABLE_ARGC && info->enable != TD_NULL) {
                return info->enable(handle, arg);
            }
            break;
        default:
            sample_effect_usage();
            break;
    }
    return EXT_FAILURE;
}

static td_s32 sample_effect_component_entry(td_s32 argc, td_char *argv[], td_bool *flag)
{
    td_s32 ret;
    effect_type_info *info = TD_NULL;
    td_void *param_data = TD_NULL;
    td_u32 param_size;
    effect_arg *arg = (effect_arg *)argv;

    if (arg->effect_type == TD_NULL) {
        return EXT_SUCCESS;
    }
    info = get_effect_info_by_type(arg->effect_type);
    if (info == TD_NULL) {
        return EXT_SUCCESS;
    }

    *flag = TD_TRUE;
    param_data = info->param_data;
    param_size = info->param_size;

    if (info->param_data == TD_NULL) {
        param_data = malloc(param_size);
        if (param_data == TD_NULL) {
            return EXT_FAILURE;
        }
    }

    ret = action_execute(info, arg, argc, param_data, param_size);

    if (info->param_data == TD_NULL && param_data != TD_NULL) {
        free(param_data);
        param_data = TD_NULL;
    }
    return ret;
}

td_s32 sample_effect(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    td_bool flag = TD_FALSE;

    if (argc <= 1) {
        sample_effect_usage();
        return EXT_SUCCESS;
    }

    ret = sample_effect_component_entry(argc, argv, &flag);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_effect_component_entry, ret);
        return ret;
    }

    if (flag == TD_TRUE) {
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_effect_exit();
    } else {
        return sample_effect_entry(argc, argv);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif