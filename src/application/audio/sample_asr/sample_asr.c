/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: asr sample
 * Author: audio
 * Create: 2025-08-27
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "thread_os.h"
#include "chip_type.h"
#include "audio_type.h"
#include "osal_list.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_ai.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"
#include "sample_audio_interface.h"

#include "audio_vqe_common_basicop_vec.h"
#include "window_coeff.h"

#include "fp16_convert.h"
#include "ctc_prefix_beamsearch.h"
#include "map_dict.h"
#include "asr.h"
#include "fbank.h"

#include "asr_ctc_model.h"
#include "nlu_model.h"
#include "nlu_slot_convert.h"
#include "nlu_slot_postproc.h"
#include "npu_drv_if.h"
#include "sample_asr.h"

/*
 *  +---------------------+---------+-----------+------------+
 *  |     NPU scratch     | data_in | nlu_input | pos_emb_pe |
 *  +---------------------+---------+-----------+------------+
 */
#define SCRATCH_NPU_SIZE              98408
#define SCRATCH_SIZE                  (SCRATCH_NPU_SIZE + 560 * 2 + 208 + 8192 * 3)

#define SCRATCH_FFBANK_35             0
#define SCRATCH_POSEMB                5600                                            /* size 8192 * 3 */
#define SCRATCH_FFBANK                (5600 + 8192 * 3)
#define SCRATCH_NLU_OUT               0
#define SCRATCH_NLU_MASK              (106 * 4)
#define SCRATCH_NLU_MODEL_OUT         (106 * 4 + 5408)                                /* size: 216, 8 align by 212 */
#define SCRATCH_NLU_OUT_SLOT          (106 * 4 + 5408 + 216)                          /* size 11450 */
#define SCRATCH_DATA_IN               (SCRATCH_NPU_SIZE)
#define SCRATCH_NLU_IN                (SCRATCH_NPU_SIZE + 560 * 2)
#define SCRATCH_POSEMB_PE             (SCRATCH_NPU_SIZE + 560 * 2 + 208)

#define CTC_OUTPUT_SIZE               20624
#define NLU_MASK_SIZE                 5408
#define FUZZ_DATA_LEN                 560

#define MIC_ORDER_REVERSE             1 /* micIn order reversed (0->NO, 1->YES) */
#define COMMAND_106_NUM               106
#define FP16_SIZE                     2

#define FUZZ_NPU_INIT_FAILED          (td_s32)0x83001001
#define FUZZ_MEM_ALLOC_FAILED         (td_s32)0x83001002
#define FUZZ_MEM_CPY_FAILED           (td_s32)0x83001003
#define FUZZ_MEM_SET_FAILED           (td_s32)0x83001004

#define FUZZ_FILE_OPEN_FAILED         (td_s32)0x83001101
#define FUZZ_FILE_READ_NULL           (td_s32)0x83001102
#define FUZZ_FILE_END                 (td_s32)0x83001103

#define FUZZ_ASR_CTC_INIT_FAILED      (td_s32)0x83001201
#define FUZZ_ASR_CTC_EXCUTE_FAILED    (td_s32)0x83001202
#define FUZZ_ASR_CTC_OUT_CPY_FAILED   (td_s32)0x83001203

#define FUZZ_NLU_OUT_CPY_FAILED       (td_s32)0x83001301
#define FUZZ_NLU_MASK_CHECKIN_ERR     (td_s32)0x83001302
#define FUZZ_NLU_INIT_ERR             (td_s32)0x83001303
#define FUZZ_NLU_EXECUTE_ERR          (td_s32)0x83001304
#define FUZZ_NLU_SLOT_OUT_ERR         (td_s32)0x83001305
#define FUZZ_NLU_SLOT_INIT_ERR        (td_s32)0x83001306

#define NLU_SLOT_MAX_SIZE             25
#define NLU_POST_MAX_SIZE             800
#define NLU_POST_OUT_MAX_SIZE         500
#define FRAME_LAP                     160
#define CHAN_NUM_IN                   1
#define HYPS_CHUNK_CUM                35
#define HYPS_POSEMB                   3
#define FRAME_CNT_RESET               3

#define FILE_NAME_LEN 100
#define ASR_RESULT_LEN 100
#define ASR_IN_PATH "/user/asr_in_16k_16bit_1ch_cmd"
#define ASR_RESULT_PATH "/user/asr_result.txt"

static td_char *date_tab[4] = { "jintian", "mingtian", "houtian", "dahoutian" };
static td_char *date_period_tab[2] = { "am", "pm" };
static td_char *distance_unit_tab[4] = { "km", "step", "lap", "m" };

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    ASR_SLOT_TYPE_DATE = 1,
    ASR_SLOT_TYPE_TIME = 2,
    ASR_SLOT_TYPE_ENERGY = 3,
    ASR_SLOT_TYPE_DISTANCE = 4
} asr_slot_type;

typedef enum {
    ASR_SLOT_DATE_TDY = 1,
    ASR_SLOT_DATE_TMR = 2,
    ASR_SLOT_DATE_DAT = 3,
    ASR_SLOT_DATE_TDAT = 4
} asr_slot_date_type;

typedef enum {
    ASR_SLOT_DATE_AM = 1,
    ASR_SLOT_DATE_PM = 2
} asr_slot_date_period;

typedef enum {
    ASR_SLOT_DIS_KM = 1,
    ASR_SLOT_DIS_STEP = 2,
    ASR_SLOT_DIS_LAP = 3,
    ASR_SLOT_DIS_M = 4
} asr_slot_distance_unit;

typedef struct {
    td_char *sample;
    td_char *sea_enable;
    td_char *fin;
} sample_asr_arg;

typedef struct {
    td_char *sample;
    td_char *ai_vol;
    td_char *ai_handle;
    td_char *integer;
    td_char *decimal;
} sample_asr_vol_arg;

typedef struct {
    td_char *scratch_asr;
    td_void *asr_coeff;
    td_s16 *data_in;
    td_u16 *ffbank_35f;
    td_u16 *pos_emb;
    td_s32 frm_cnt;
    td_void *asr_in;
    bool isfirst;
    td_s32 chunk_num;
    td_void *cur_hyps;
    td_float *nlu_out_vec;
    td_s16 *pps_data_tmp[FUZZ_FRAME_LEN];
    td_void *nlu_input;
    td_s32 nlu_out_index;
    td_char *nlu_out_slot_data;
    td_s16 len_cur_hyps_last;
    td_s16 len_cur_hyps_same_num;

    td_bool dfx;
    td_u32 fcount;
    FILE *fp_asr_in;
    FILE *fp_asr_result;
} asr_inst;

typedef struct sample_asr_inst {
    td_char *scratch;
    td_bool sea_enable;
    asr_inst *asr_st;
    athread_handle task_asr;
    td_bool task_asr_enable;

    td_handle h_adp;
    td_handle h_ai;
    td_handle h_sea;
    uapi_ai_port ai_port;
    uapi_audio_pcm_format pcm_format;
    FILE *fp_fuzz;

    struct osal_list_head node;
} sample_asr_inst;

static td_s16 __attribute((aligned(8))) g_pps_data_asr[CHAN_NUM_MAX][FUZZ_FRAME_LEN] = { 0 };

#define FUZZ_ASR_MODEL        "/user/asr/new_ctc_final_fifo.exeom"
#define FUZZ_NLU_MODEL        "/user/asr/NLU.exeom"
#define FUZZ_ASR_MODEL_OFFSET "/user/asr/offset/offset.bin"
#define FUZZ_POSEMB_NAME      "/user/asr/save_posemb5000x64_uint16.bin"

static td_void *g_ctc_model_output = TD_NULL;
static td_char g_nlu_postproc_in[NLU_POST_MAX_SIZE];
static td_char g_nlu_postproc_index[NLU_POST_MAX_SIZE];
static td_char g_nlu_postproc_out[NLU_POST_OUT_MAX_SIZE];
static td_s32 g_nlu_out_slot_data[NLU_SLOT_MAX_SIZE];

static OSAL_LIST_HEAD(g_sample_asr_list);

td_u32 sample_asr_get_inst_size(td_void)
{
    return sizeof(asr_inst);
}

td_u32 sample_asr_get_scratch_size(td_void)
{
    return SCRATCH_SIZE;
}

td_void sample_asr_set_scratch(td_void *inst, td_char *buf)
{
    asr_inst *st = (asr_inst *)inst;
    st->scratch_asr = buf;
}

static sample_asr_inst *sample_asr_alloc_inst(td_void)
{
    sample_asr_inst *inst = (sample_asr_inst *)malloc(sizeof(sample_asr_inst));
    if (inst == TD_NULL) {
        sap_printf("sample asr inst mem alloc failed\n");
        return TD_NULL;
    }

    (td_void)memset_s(inst, sizeof(*inst), 0, sizeof(sample_asr_inst));
    inst->asr_st = (asr_inst *)malloc(sizeof(asr_inst));
    if (inst->asr_st == TD_NULL) {
        sap_printf("asr inst mem alloc failed\n");
        free(inst);
        inst = TD_NULL;
        return TD_NULL;
    }
    inst->scratch = (td_char *)malloc(SCRATCH_SIZE);
    if (inst->scratch == TD_NULL) {
        sap_printf("asr scratch mem alloc failed\n");
        free(inst->asr_st);
        inst->asr_st = TD_NULL;
        free(inst);
        inst = TD_NULL;
        return TD_NULL;
    }
    inst->asr_st->scratch_asr = inst->scratch;

    osal_list_add_tail(&inst->node, &g_sample_asr_list);
    return inst;
}

static td_void sample_asr_close_file(sample_asr_inst *inst)
{
    if (inst->fp_fuzz != TD_NULL) {
        (td_void)fclose(inst->fp_fuzz);
        inst->fp_fuzz = TD_NULL;
    }
}

static td_void sample_asr_free_inst(sample_asr_inst *inst)
{
    if (inst->asr_st != TD_NULL) {
        free(inst->asr_st);
        inst->asr_st = TD_NULL;
    }
    if (inst->scratch != TD_NULL) {
        free(inst->scratch);
        inst->scratch = TD_NULL;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static sample_asr_inst *sample_asr_get_inst(td_void)
{
    if (osal_list_empty(&g_sample_asr_list) != TD_FALSE) {
        return TD_NULL;
    }

    return osal_list_first_entry(&g_sample_asr_list, sample_asr_inst, node);
}

static td_s32 asr_event_proc(td_handle ai, uapi_ai_event_type event, const td_void *param, td_void *context)
{
    sample_asr_inst *test_inst = (sample_asr_inst *)context;

    sap_err_log_h32(event);

    sap_unused(ai);
    sap_unused(param);
    sap_unused(test_inst);
    return EXT_SUCCESS;
}

/* fuzz asr */
static td_void channel_de_interlace(const td_s16 *in, td_s16 **out1, td_s32 channels, td_s32 frm_len)
{
    td_s32 i, j;
    td_s32 src_index = 0;

    for (i = 0; i < frm_len; i++, src_index += channels) {
        for (j = 0; j < channels; j++) {
#if MIC_ORDER_REVERSE
            out1[j][i] = in[src_index + channels - 1 - j]; /* micIn order reverse */
#else
            out1[j][i] = in[src_index + j];
#endif
        }
    }
}

static td_void intent_result_print(nlu_stru *nlu_line, td_char *str_result)
{
    td_s32 i, ret;
    sap_printf("nlu_postproc_in: %s\n\n", g_nlu_postproc_index);
    sap_printf("nlu_postproc_out: %s\n\n", g_nlu_postproc_out);

    sap_printf("asr text: ");
    for (i = 0; i < nlu_line->sentence_len; i++) {
        sap_printf("%s ", nlu_line->asr_text[i]);
    }
    sap_printf("\nbio label: ");
    for (i = 0; i < nlu_line->sentence_len; i++) {
        sap_printf("%s ", nlu_line->bio_label[i]);
    }
    ret = sprintf_s(str_result + strlen(str_result), ASR_RESULT_LEN - strlen(str_result),
        "\nIntent: %s\n", nlu_line->intent_pred);
    if (ret < 0) {
        sap_err_log_info("asr result write intent error");
    }
    ret = sprintf_s(str_result + strlen(str_result), ASR_RESULT_LEN - strlen(str_result),
        "\nslot type: %u \n", nlu_line->slot_type);
    if (ret < 0) {
        sap_err_log_info("asr result write slot_type error");
    }
}

static td_void slot_date_print(nlu_stru *nlu_line, td_char *str_result)
{
    if (sprintf_s(str_result + strlen(str_result), ASR_RESULT_LEN - strlen(str_result),
        "date: %s, am_or_pm: %s, alarm_hour: %u, alarm_minute: %u\n",
        date_tab[(td_u32)nlu_line->nlu_slot->datetime_stru.date - 1],
        date_period_tab[(td_u32)nlu_line->nlu_slot->datetime_stru.am_or_pm - 1],
        nlu_line->nlu_slot->datetime_stru.alarm_hour,
        nlu_line->nlu_slot->datetime_stru.alarm_minute) < 0) {
        sap_err_log_info("asr result write date error");
    }
}

static td_void slot_time_print(nlu_stru *nlu_line, td_char *str_result)
{
    if (sprintf_s(str_result + strlen(str_result), ASR_RESULT_LEN - strlen(str_result),
        "hours: %u, minutes: %u, seconds: %u\n",
        nlu_line->nlu_slot->timedur_stru.hours,
        nlu_line->nlu_slot->timedur_stru.minutes,
        nlu_line->nlu_slot->timedur_stru.seconds) < 0) {
        sap_err_log_info("asr result write time error");
    }
}

static td_void slot_energy_print(nlu_stru *nlu_line, td_char *str_result)
{
    if (sprintf_s(str_result + strlen(str_result), ASR_RESULT_LEN - strlen(str_result),
        "energy: %.1f, energy_unit: kcal\n",
        nlu_line->nlu_slot->energy_stru.amount) < 0) {
        sap_err_log_info("asr result write energy error");
    }
}

static td_void slot_distance_print(nlu_stru *nlu_line, td_char *str_result)
{
    if (sprintf_s(str_result + strlen(str_result), ASR_RESULT_LEN - strlen(str_result),
        "distance: %.1f, %s",
        nlu_line->nlu_slot->distance_stru.amount,
        distance_unit_tab[(td_u32)nlu_line->nlu_slot->distance_stru.dis_unit - 1]) < 0) {
        sap_err_log_info("asr result write distance error");
    }
}

static td_void asr_fuzz_save_result(asr_inst *st, td_void *handle, td_char *str_result)
{
    td_s32 i;
    nlu_stru *nlu_line = (nlu_stru *)handle;

    /* save post in data */
    (td_void)fprintf(st->fp_asr_result, "\nfuzz_cmd[%u]\n", st->fcount);
    (td_void)fprintf(st->fp_asr_result, "post in: %s\nasr text: ", g_nlu_postproc_index);

    /* save asr text */
    for (i = 0; i < nlu_line->sentence_len; i++) {
        (td_void)fprintf(st->fp_asr_result, "%s ", nlu_line->asr_text[i]);
    }
    (td_void)fprintf(st->fp_asr_result, "\nbio label: ");

    /* save bio label */
    for (i = 0; i < nlu_line->sentence_len; i++) {
        (td_void)fprintf(st->fp_asr_result, "%s ", nlu_line->bio_label[i]);
    }
    (td_void)fprintf(st->fp_asr_result, "\nasr result: ");

    /* save asr result */
    (td_void)fprintf(st->fp_asr_result, "%s\n", str_result);

    if (st->fp_asr_in != TD_NULL) {
        (td_void)fclose(st->fp_asr_in);
        st->fp_asr_in = TD_NULL;
    }
}

static td_void result_print(asr_inst *st, td_void *handle)
{
    nlu_stru *nlu_line = (nlu_stru *)handle;
    td_char str_result[ASR_RESULT_LEN];

    intent_result_print(nlu_line, (td_char *)str_result);

    switch (nlu_line->slot_type) {
        case ASR_SLOT_TYPE_DATE:
            slot_date_print(nlu_line, (td_char *)str_result);
            break;
        case ASR_SLOT_TYPE_TIME:
            slot_time_print(nlu_line, (td_char *)str_result);
            break;
        case ASR_SLOT_TYPE_ENERGY:
            slot_energy_print(nlu_line, (td_char *)str_result);
            break;
        case ASR_SLOT_TYPE_DISTANCE:
            slot_distance_print(nlu_line, (td_char *)str_result);
            break;
        default:
            (td_void)sprintf_s(str_result + strlen(str_result), ASR_RESULT_LEN - strlen(str_result), "No slot\n");
            break;
    }
    sap_printf("%s\n", str_result);
    if (st->dfx && st->fp_asr_result != TD_NULL) {
        asr_fuzz_save_result(st, nlu_line, (td_char *)str_result);
    }
}

static td_void asr_fuzz_hpys_init(asr_inst *st, long_hyps *cur_hyps)
{
    cur_hyps->candidate[0].n_pb = 0;
    cur_hyps->candidate[0].n_pnb = NEG_INF_VALUE;
    cur_hyps->candidate[0].len = 0;
    cur_hyps->len = 1;

    st->len_cur_hyps_last = 0;
    st->len_cur_hyps_same_num = 0;
}

static td_void asr_fuzz_dfx_init(asr_inst *st, td_bool dfx)
{
    if (dfx == TD_FALSE) {
        st->dfx = TD_FALSE;
        return;
    }
    st->dfx = TD_TRUE;
    st->fcount = 0;

    st->fp_asr_result = fopen(ASR_RESULT_PATH, "w");
    if (st->fp_asr_result == TD_NULL) {
        sap_err_log_info("file asr_result open failed");
    }
}

static td_void asr_init_set_frm_attr(asr_inst *st)
{
    st->isfirst = true;
    st->frm_cnt = 0;
    st->chunk_num = 0;
}

td_s32 asr_fuzz_init(td_void *inst, td_bool dfx)
{
    asr_inst *st = (asr_inst *)inst;
    FILE *file_pos_emb = TD_NULL;
    asr_pre_post_para *asr_coeff;
    stru_asr_in *asr_in;
    long_hyps *cur_hyps;
    td_u32 rlen;

    uapi_npu_dlog_setlevel(0, 0x3, 1);

    if (initModelEnv() != SUCCESS) {
        return FUZZ_NPU_INIT_FAILED;
    }

    st->asr_coeff = malloc(sizeof(asr_pre_post_para));
    asr_coeff = (asr_pre_post_para *)st->asr_coeff;
    if (asr_coeff == TD_NULL) {
        return FUZZ_MEM_ALLOC_FAILED;
    }

    st->asr_in = malloc(sizeof(stru_asr_in));
    asr_in = (stru_asr_in *)st->asr_in;
    if (asr_in == TD_NULL) {
        return FUZZ_MEM_ALLOC_FAILED;
    }

    st->cur_hyps = malloc(sizeof(long_hyps));
    cur_hyps = (long_hyps *)st->cur_hyps;
    if (cur_hyps == TD_NULL) {
        return FUZZ_MEM_ALLOC_FAILED;
    }
    if (memset_s(cur_hyps, sizeof(long_hyps), 0, sizeof(long_hyps)) != EOK) {
        return FUZZ_MEM_SET_FAILED;
    }
    asr_fuzz_hpys_init(st, cur_hyps);

    asr_coeff->pos_emb_pe = (td_u16 *)(st->scratch_asr + SCRATCH_POSEMB_PE);

    file_pos_emb = fopen(FUZZ_POSEMB_NAME, "rb");
    if (file_pos_emb == TD_NULL) {
        return FUZZ_FILE_OPEN_FAILED;
    }
    rlen = fread(asr_coeff->pos_emb_pe, sizeof(imedia_uint16), HYPS_POSEMB * POS_EMB_ROW * POS_EMB_COL, file_pos_emb);
    if (rlen == 0) {
        (td_void)fclose(file_pos_emb);
        return FUZZ_FILE_READ_NULL;
    }
    (td_void)fclose(file_pos_emb);
    file_pos_emb = TD_NULL;

    st->data_in = (td_s16 *)(st->scratch_asr + SCRATCH_DATA_IN);

    audio_vqe_common_vec_set_int16(st->data_in, FUZZ_DATA_LEN, 0);

    st->nlu_input = (td_void *)(st->scratch_asr + SCRATCH_NLU_IN);

    asr_init_set_frm_attr(st);
    asr_fuzz_dfx_init(st, dfx);

    return FUZZ_SUCCESS;
}

static td_void asr_fuzz_dfx_deinit(asr_inst *st)
{
    if (st->fp_asr_result != TD_NULL) {
        (td_void)fclose(st->fp_asr_result);
        st->fp_asr_result = TD_NULL;
    }
}

td_s32 asr_fuzz_deinit(td_void *inst)
{
    asr_inst *st = (asr_inst *)inst;
    asrctcModelDestroy();
    if (((asr_pre_post_para *)st->asr_coeff)->ctc_out != TD_NULL) {
        free(((asr_pre_post_para *)st->asr_coeff)->ctc_out);
        ((asr_pre_post_para *)st->asr_coeff)->ctc_out = TD_NULL;
    }
    if (st->asr_coeff != TD_NULL) {
        free(st->asr_coeff);
        st->asr_coeff = TD_NULL;
    }

    if (st->asr_in != TD_NULL) {
        free(st->asr_in);
        st->asr_in = TD_NULL;
    }

    if (st->cur_hyps != TD_NULL) {
        free(st->cur_hyps);
        st->cur_hyps = TD_NULL;
    }

    if (g_ctc_model_output != TD_NULL) {
        free(g_ctc_model_output);
        g_ctc_model_output = TD_NULL;
    }
    if (st->dfx) {
        asr_fuzz_dfx_deinit(st);
    }

    return destroyModelEnv();
}

static td_s32 fuzz_nlu_output_proc(asr_inst *st, td_float *nlu_out_vec)
{
    td_s32 i;
    td_s16 *nlu_temp;
    td_void *nlu_model_output = (td_void *)(st->scratch_asr + SCRATCH_NLU_MODEL_OUT); /* size: 212 */

    if (nluModelCopyOutput(nlu_model_output) != SUCCESS) {
        nluModelDestroy();
        return FUZZ_NLU_OUT_CPY_FAILED;
    }

    /* nlu slot */
    st->nlu_out_slot_data = st->scratch_asr + SCRATCH_NLU_OUT_SLOT;
    if (nluModelCopyOutputSlot((td_void *)st->nlu_out_slot_data) != SUCCESS) {
        return FUZZ_NLU_SLOT_OUT_ERR;
    }

    nluModelDestroy();

    nlu_temp = (td_s16 *)nlu_model_output;
    for (i = 0; i < COMMAND_106_NUM; i++) {
        fp162fp32(&nlu_out_vec[i], nlu_temp[i]);
    }

    return FUZZ_SUCCESS;
}

static td_void fuzz_nlu_calc_index(asr_inst *st, td_float *nlu_out_vec)
{
    td_s32 i;
    td_float nlu_max = 0;
    st->nlu_out_index = 2; /* default 2, ukn */
    nlu_max = nlu_out_vec[2];

    for (i = 3; i < COMMAND_106_NUM; i++) { /* from 3 to 106 */
        if (nlu_max < nlu_out_vec[i]) {
            st->nlu_out_index = i;
            nlu_max = nlu_out_vec[i];
        }
    }
}

static td_s32 fuzz_nlu(asr_inst *st, stru_asr_in *asr_in)
{
    /* nlu model */
    td_s32 ret;
    nlu_in *nlu_input = (nlu_in *)st->nlu_input;

    td_float *nlu_out_vec = (td_float *)(st->scratch_asr + SCRATCH_NLU_OUT); /* size: 106 * 4 */

    if (nluModelInit(FUZZ_NLU_MODEL) != SUCCESS) {
        return FUZZ_NLU_INIT_ERR;
    }

    if (nluModelExecute((td_void *)nlu_input->seq_data, nlu_input->seq_len) != SUCCESS) {
        nluModelDestroy();
        return FUZZ_NLU_EXECUTE_ERR;
    }

    ret = fuzz_nlu_output_proc(st, nlu_out_vec);
    if (ret != FUZZ_SUCCESS) {
        return ret;
    }

    fuzz_nlu_calc_index(st, nlu_out_vec);
    st->chunk_num = 0;
    asr_in->ctc_flag = 0;

    return FUZZ_SUCCESS;
}

static td_void fuzz_ctc_calc_seq(nlu_in *nlu_input, long_hyps *cur_hyps)
{
    td_s32 i;
    for (i = 0; i < NLU_INPUT_LENGTH; i++) {
        nlu_input->seq_data[i] = 1;
    }
    nlu_input->seq_len = 0;
    if ((cur_hyps->len != 0) && (cur_hyps->candidate[0].len != 0)) {
        for (i = 0; i < cur_hyps->candidate[0].len; i++) {
            nlu_input->seq_data[i] = map_asr2nlu[cur_hyps->candidate[0].prefix[i]];
        }
        nlu_input->seq_len = cur_hyps->candidate[0].len;

        for (i = cur_hyps->candidate[0].len; i < NLU_INPUT_LENGTH; i++) {
            nlu_input->seq_data[i] = 1;
        }
    }
}

static td_s32 fuzz_ctc(asr_inst *st)
{
    /* ctc */
    td_s32 i, j;
    long_hyps *cur_hyps = (long_hyps *)st->cur_hyps;
    asr_pre_post_para *asr_coeff = (asr_pre_post_para *)st->asr_coeff;

    g_ctc_model_output = malloc(CTC_OUTPUT_SIZE);
    if (g_ctc_model_output == TD_NULL) {
        asrctcModelDestroy();
        return FUZZ_MEM_ALLOC_FAILED;
    }
    if (asrctcModelCopyOutput(g_ctc_model_output) != SUCCESS) {
        free(g_ctc_model_output);
        g_ctc_model_output = TD_NULL;
        asrctcModelDestroy();
        return FUZZ_ASR_CTC_OUT_CPY_FAILED;
    }

    asr_coeff->ctc_out = (td_float *)malloc(CTC_CHUNK_SIZE_8 * CTC_CHAR_LEN_1289 * sizeof(td_float));
    if (asr_coeff->ctc_out == TD_NULL) {
        free(g_ctc_model_output);
        g_ctc_model_output = TD_NULL;
        return FUZZ_MEM_ALLOC_FAILED;
    }

    for (i = 0; i < CTC_CHUNK_SIZE_8; i++) {
        for (j = 0; j < CTC_CHAR_LEN_1289; j++) {
            fp162fp32(&asr_coeff->ctc_out[i * CTC_CHAR_LEN_1289 + j],
                ((td_s16 *)g_ctc_model_output)[i * CTC_CHAR_LEN_1289 + j]);
        }
    }

    free(g_ctc_model_output);
    g_ctc_model_output = TD_NULL;

    ctc_prefix_beamsearch(asr_coeff->ctc_out, cur_hyps, st->chunk_num);

    free(asr_coeff->ctc_out);
    asr_coeff->ctc_out = TD_NULL;

    return FUZZ_SUCCESS;
}

static td_s32 fuzz_nlu_proc(asr_inst *st, stru_asr_in *asr_in)
{
    td_s32 ret;
    td_void *handle = TD_NULL;

    ret = fuzz_nlu(st, asr_in);
    if (ret != FUZZ_SUCCESS) {
        return ret;
    }

    /* post proc slot */
    getSlotMax((td_float *)st->nlu_out_slot_data, ((nlu_in *)st->nlu_input)->seq_len, g_nlu_out_slot_data);

    (td_void)memset_s(g_nlu_postproc_in, NLU_POST_MAX_SIZE, 0, NLU_POST_MAX_SIZE);
    (td_void)memset_s(g_nlu_postproc_out, NLU_POST_OUT_MAX_SIZE, 0, NLU_POST_OUT_MAX_SIZE);
    nlu_net_mapto_postproc(((nlu_in *)st->nlu_input)->seq_data, st->nlu_out_index,
        g_nlu_out_slot_data, ((nlu_in *)st->nlu_input)->seq_len, g_nlu_postproc_in);
    (td_void)memcpy_s(g_nlu_postproc_index, NLU_POST_MAX_SIZE, g_nlu_postproc_in, NLU_POST_MAX_SIZE);

    ret = slu_slot_convert_init(&handle, 0);
    if (ret != FUZZ_SUCCESS) {
        return FUZZ_NLU_SLOT_INIT_ERR;
    }
    nlu_slot_postproc(handle, g_nlu_postproc_in, g_nlu_postproc_out);

    result_print(st, handle);

    slu_slot_convert_deinit(handle, 0);

    return FUZZ_WORD_DONE;
}

static td_void fuzz_dfx_enable(asr_inst *st)
{
    td_s32 ret;
    char asr_file[FILE_NAME_LEN] = ASR_IN_PATH;
    char *asr_file_in = asr_file;
    if (st->dfx == TD_FALSE) {
        return;
    }

    st->fcount++;
    ret = sprintf_s(asr_file + strlen(ASR_IN_PATH), FILE_NAME_LEN - strlen(ASR_IN_PATH), "[%u].pcm", st->fcount);
    if (ret < 0) {
        sap_err_log_info("asr stream in file name create error");
    }
    st->fp_asr_in = sample_audio_open_output_stream(asr_file_in);
    if (st->fp_asr_in == TD_NULL) {
        sap_err_log_info("file asr_in open failed");
    }
}

static td_s32 fuzz_proc_check_first(asr_inst *st)
{
    if (st->isfirst) {
        st->ffbank_35f = (td_u16 *)(st->scratch_asr + SCRATCH_FFBANK_35);
        st->pos_emb = (td_u16 *)(st->scratch_asr + SCRATCH_POSEMB);

        if (asrctcModelInit(FUZZ_ASR_MODEL, FUZZ_ASR_MODEL_OFFSET) != SUCCESS) {
            return FUZZ_ASR_CTC_INIT_FAILED;
        }
        asrctcModelCnnCacheReset();
        fuzz_dfx_enable(st);

        st->isfirst = false;
    }
    return FUZZ_SUCCESS;
}

static td_void fuzz_hyps_detect(long_hyps *cur_hyps, asr_inst *st, stru_asr_in *asr_in)
{
    td_s32 len_hyps = cur_hyps->candidate[0].len;
    if (((len_hyps == st->len_cur_hyps_last) && (len_hyps != 0))) {
        st->len_cur_hyps_same_num += 1;
    } else {
        st->len_cur_hyps_same_num = 0;
    }

    if (st->len_cur_hyps_same_num >= 3) { /* start from 3 frm */
        asr_in->ctc_flag = 1;
    }

    st->len_cur_hyps_last = len_hyps;
    st->chunk_num = st->chunk_num + 1;
}

static td_s32 fuzz_npu_proc(asr_inst *st)
{
    td_s32 ret = FUZZ_SUCCESS;
    td_u16 *ffbank;
    stru_asr_in *asr_in = (stru_asr_in *)st->asr_in;
    asr_pre_post_para *asr_coeff = (asr_pre_post_para *)st->asr_coeff;
    long_hyps *cur_hyps = (long_hyps *)st->cur_hyps;
    nlu_in *nlu_input = (nlu_in *)st->nlu_input;

    /* 2 frm */
    audio_vqe_common_vec_copy_int16_to_int16(st->pps_data_tmp[0], FUZZ_FRAME_LEN, &st->data_in[FUZZ_FRAME_LEN * 2]);
    ffbank = (td_u16 *)(st->scratch_asr + SCRATCH_FFBANK);

    cal_ffbank(st->data_in, ffbank);
    get_encoder_input(ffbank, asr_in, asr_coeff);

    if (asr_in->flag == 1) { /* 35frm for asr inference */
        if (asrctcModelExecute((td_void *)asr_in->fbank_in, (td_void *)asr_in->pos_emb) != SUCCESS) {
            asrctcModelDestroy();
            return FUZZ_ASR_CTC_EXCUTE_FAILED;
        }

        ret = fuzz_ctc(st);
        if (ret != FUZZ_SUCCESS) {
            return ret;
        }

        fuzz_hyps_detect(cur_hyps, st, asr_in);

        if (asr_in->ctc_flag) {
            asrctcModelDestroy();

            fuzz_ctc_calc_seq(nlu_input, cur_hyps);
            ret = fuzz_nlu_proc(st, asr_in);
            if (ret != FUZZ_SUCCESS && ret != FUZZ_WORD_DONE) {
                sap_err_log_fun(fuzz_nlu_proc, ret);
            }

            st->isfirst = true;
            asr_in->ctc_flag = 0;
            asr_in->frm_cnt = 0;

            memset_s(cur_hyps, sizeof(long_hyps), 0, sizeof(long_hyps));
            cur_hyps->candidate[0].n_pb = 0;
            cur_hyps->candidate[0].n_pnb = NEG_INF_VALUE;
            cur_hyps->candidate[0].len = 0;
            cur_hyps->len = 1;

            st->len_cur_hyps_last = 0;
        }
    }
    audio_vqe_common_vec_copy_int16_to_int16(&st->data_in[FUZZ_FRAME_LEN], FUZZ_FRAME_LEN << 1, st->data_in);
    /* first 3 frms are saved for encoder, 32768 max */
    st->frm_cnt = st->frm_cnt > 32768 ? FRAME_CNT_RESET : (st->frm_cnt + 1);
    return ret;
}

static td_void asr_fuzz_save_stream(asr_inst *st, td_s16 *stream_in, td_u32 read_len)
{
    td_u32 ret;
    if (st->fp_asr_in == TD_NULL) {
        return;
    }
    ret = fwrite(stream_in, sizeof(td_s16), read_len, st->fp_asr_in);
    if (ret != read_len) {
        sap_err_log_info("write asr_in error");
    }
}

td_s32 asr_fuzz_proc(td_void *inst, td_s16 *stream_in, td_u32 read_len)
{
    asr_inst *st = (asr_inst *)inst;
    td_s32 ret;
    stru_asr_in *asr_in = (stru_asr_in *)st->asr_in;

    ret = fuzz_proc_check_first(st);
    if (ret != FUZZ_SUCCESS) {
        return ret;
    }
    if (read_len < FRAME_LAP * CHAN_NUM_IN) {
        asr_in->ctc_flag = 0;
        return FUZZ_WORD_DONE;
    }
    if (st->dfx) {
        asr_fuzz_save_stream(st, stream_in, read_len);
    }
    channel_de_interlace((const td_s16 *)stream_in, st->pps_data_tmp, CHAN_NUM_IN, FRAME_LAP);

    if (st->frm_cnt == 0) {
        get_encoder_init(asr_in, st->ffbank_35f, st->pos_emb);
    }
    if (st->frm_cnt < 2) { /* 2 frm, save 20ms input */
        audio_vqe_common_vec_copy_int16_to_int16(st->pps_data_tmp[0],
            FUZZ_FRAME_LEN, &st->data_in[FUZZ_FRAME_LEN * st->frm_cnt]);
        st->frm_cnt++;
        return FUZZ_SUCCESS;
    } else {
        return fuzz_npu_proc(st);
    }

    return FUZZ_SUCCESS;
}

static td_s32 sample_asr_thread_task(td_void *asr, td_handle h_adp, FILE *fp_fuzz)
{
    asr_inst *st = (asr_inst *)asr;
    td_s32 ret, ret_fuzz;
    td_u32 read_len;
    uapi_audio_frame frame;
    td_s16 fin_buf[FUZZ_FRAME_LEN];

    if (fp_fuzz != TD_NULL) {
        read_len = fread((td_char *)fin_buf, sizeof(td_s16), FUZZ_FRAME_LEN, fp_fuzz);
        ret_fuzz = asr_fuzz_proc(st, (td_s16 *)fin_buf, read_len);
        if (ret_fuzz != FUZZ_SUCCESS && ret_fuzz != FUZZ_WORD_DONE) {
            sap_err_log_fun(asr_fuzz_proc, ret_fuzz);
        }
        if (read_len < FUZZ_FRAME_LEN) {
            return FUZZ_FILE_END;
        }
    } else {
        ret = uapi_adp_acquire_frame(h_adp, &frame);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS);
            return ret;
        }
        read_len = frame.bits_bytes >> 1;

        ret_fuzz = asr_fuzz_proc(st, (td_s16 *)frame.bits_buffer, read_len);
        if (ret_fuzz != FUZZ_SUCCESS && ret_fuzz != FUZZ_WORD_DONE) {
            sap_err_log_fun(asr_fuzz_proc, ret_fuzz);
        }
        ret = uapi_adp_release_frame(h_adp, &frame);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_release_frame, ret);
        }
    }
    return ret_fuzz;
}

td_void sample_asr_set_pps_data(td_void *inst, td_s16 pps_data[][FUZZ_FRAME_LEN])
{
    td_s32 i;
    asr_inst *st = (asr_inst *)inst;

    for (i = 0; i < CHAN_NUM_MAX; i++) {
        st->pps_data_tmp[i] = (td_s16 *)pps_data[i];
    }
}

static td_void sample_asr_thread(td_void *args)
{
    td_s32 ret, i;
    asr_inst *st;
    sample_asr_inst *inst = (sample_asr_inst *)args;
    if (inst == TD_NULL) {
        sap_err_log_info("ai_inst is null");
        return;
    }

    st = inst->asr_st;

    ret = asr_fuzz_init(st, TD_FALSE);
    if (ret != FUZZ_SUCCESS) {
        sap_err_log_fun(asr_fuzz_init, ret);
        goto deinit;
    }

    for (i = 0; i < CHAN_NUM_MAX; i++) {
        st->pps_data_tmp[i] = (td_s16 *)g_pps_data_asr[i];
    }

    while (inst->task_asr_enable) {
        ret = sample_asr_thread_task(st, inst->h_adp, inst->fp_fuzz);
        if (ret == FUZZ_FILE_END) {
            break;
        }
    }

deinit:
    ret = asr_fuzz_deinit(st);
    if (ret != FUZZ_SUCCESS) {
        sap_err_log_fun(asr_fuzz_deinit, ret);
    }

    athread_set_exit(inst->task_asr, TD_TRUE);
}

static td_s32 sample_asr_sea_init(td_void)
{
    td_s32 ret;

    ret = uapi_sea_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_init, ret);
        return ret;
    }

    ret = uapi_sea_load_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_load_engine, ret);
        (td_void)uapi_sea_deinit();
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_asr_sys_init(sample_asr_inst *inst)
{
    td_s32 ret;

    ret = uapi_adp_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_init, ret);
        return ret;
    }

    ret = uapi_ai_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_init, ret);
        goto out;
    }

    if (inst->sea_enable) {
        ret = sample_asr_sea_init();
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_asr_sea_init, ret);
            goto out1;
        }
    }

    return EXT_SUCCESS;
out1:
    (td_void)uapi_ai_deinit();
out:
    (td_void)uapi_adp_deinit();
    return ret;
}

static td_void sample_asr_sea_deinit(td_void)
{
    td_s32 ret;

    ret = uapi_sea_unload_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_unload_engine, ret);
    }

    ret = uapi_sea_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_deinit, ret);
    }
}

static td_void sample_asr_sys_deinit(sample_asr_inst *inst)
{
    td_s32 ret;

    ret = uapi_ai_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_deinit, ret);
    }
    if (inst->sea_enable) {
        sample_asr_sea_deinit();
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_deinit, ret);
    }
}

static td_void sample_asr_close_adp(sample_asr_inst *inst)
{
    if (inst->h_adp != 0) {
        uapi_adp_destroy(inst->h_adp);
        inst->h_adp = 0;
    }
}

static td_s32 sample_asr_open_adp(sample_asr_inst *inst)
{
    td_s32 ret;
    uapi_adp_attr adp_attr;

    inst->h_adp = 0;

    ret = uapi_adp_get_def_attr(&adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_get_def_attr, ret);
        return ret;
    }

    ret = uapi_adp_create(&inst->h_adp, &adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_create, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_asr_close_ai(sample_asr_inst *inst)
{
    td_s32 ret;

    if (inst->h_ai == 0) {
        return;
    }

    /* Detach ADP instance from AI instance */
    if (inst->h_sea != 0) {
        ret = uapi_ai_detach_output(inst->h_ai, inst->h_sea);
    } else {
        ret = uapi_ai_detach_output(inst->h_ai, inst->h_adp);
    }
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_detach_output, ret);
    }

    ret = uapi_ai_close(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_close, ret);
    }

    inst->h_ai = 0;
}

static td_s32 sample_asr_open_ai(sample_asr_inst *inst)
{
    td_s32 ret;
    uapi_ai_attr ai_attr;

    ret = uapi_ai_get_default_attr(inst->ai_port, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_get_default_attr, ret);
        return ret;
    }

    ret = uapi_ai_open(&inst->h_ai, inst->ai_port, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_open, ret);
        return ret;
    }

    /* Register AI event process callback */
    ret = uapi_ai_register_event_proc(inst->h_ai, asr_event_proc, (td_void *)inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_register_event_proc, ret);
        goto out;
    }

    /* Attach ADP instance to AI instance */
    if (inst->h_sea != 0) {
        ret = uapi_ai_attach_output(inst->h_ai, inst->h_sea);
    } else {
        ret = uapi_ai_attach_output(inst->h_ai, inst->h_adp);
    }
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_ai_close(inst->h_ai);
    return ret;
}

static td_s32 sample_asr_inst_init(const sample_asr_arg *arg, sample_asr_inst *inst)
{
    inst->ai_port = AI_PORT_DEFAULT;

    if (arg->sea_enable == TD_NULL) {
        sap_err_log_info("sea enable not set\n");
        return EXT_FAILURE;
    }
    inst->sea_enable = (td_bool)(*arg->sea_enable - '0');

    if (arg->fin != TD_NULL) {
        inst->fp_fuzz = sample_audio_open_input_stream(arg->fin);
        if (inst->fp_fuzz == TD_NULL) {
            sap_printf("open file [%s] failed\n", arg->fin);
            return EXT_FAILURE;
        }
        sap_printf("open file [%s] success\n", arg->fin);
    } else {
        inst->fp_fuzz = TD_NULL;
    }

    inst->pcm_format.channels = UAPI_AUDIO_CHANNEL_1;
    inst->pcm_format.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    inst->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */
    return EXT_SUCCESS;
}

static td_void sample_asr_close_sea(sample_asr_inst *inst)
{
    td_s32 ret;

    if (inst->h_sea == 0) {
        return;
    }

    ret = uapi_sea_detach_output(inst->h_sea, UAPI_SEA_OUTPUT_ASR_SRC, inst->h_adp);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_detach_output, ret);
    }

    ret = uapi_sea_destroy(inst->h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_destroy, ret);
    }

    inst->h_sea = 0;
}

static td_void sample_asr_close_inst(sample_asr_inst *inst)
{
    sample_asr_close_ai(inst);
    sample_asr_close_sea(inst);
    sample_asr_close_adp(inst);
    sample_asr_sys_deinit(inst);
}

static td_s32 sample_asr_open_sea(sample_asr_inst *inst)
{
    td_s32 ret;
    uapi_sea_attr sea_attr;
    uapi_sea_eng_sel sea_eng;
    uapi_sea_afe_attr afe_attr = {
        .type = UAPI_SEA_AFE_SEE,
        .mode = UAPI_SEA_NORM_MODE,
        .u.see.options = UAPI_SEA_AFE_OPT_AGC,
    };

    inst->h_sea = 0;

    /* Create SEA instance */
    (td_void)memset_s(&sea_eng, sizeof(sea_eng), 0, sizeof(sea_eng));
    sea_eng.afe.type = UAPI_SEA_AFE_SEE;
    sea_eng.afe.lib_id = UAPI_SEA_LIB_SEE;
    ret = uapi_sea_get_default_attr(&sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_get_default_attr, ret);
        return ret;
    }

    sea_attr.in_pcm.bit_depth = inst->pcm_format.bit_depth;
    sea_attr.in_pcm.channels = inst->pcm_format.channels;
    sea_attr.in_pcm.sample_rate = inst->pcm_format.sample_rate;
    sea_attr.in_pcm.sample_per_frame = inst->pcm_format.sample_per_frame;
    sea_attr.ref_pcm.channels = 0;
    ret = uapi_sea_create(&inst->h_sea, &sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_create, ret);
        return ret;
    }

    ret = uapi_sea_set_afe_attr(inst->h_sea, UAPI_SEA_AFE_SEE, &afe_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_set_afe_attr, ret);
        goto out;
    }

    ret = uapi_sea_attach_output(inst->h_sea, UAPI_SEA_OUTPUT_ASR_SRC, inst->h_adp);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_sea_destroy(inst->h_sea);
    return ret;
}

static td_s32 sample_asr_open_inst(sample_asr_inst *inst)
{
    td_s32 ret;

    ret = sample_asr_sys_init(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_asr_sys_init, ret);
        return ret;
    }

    /*
     * 数据流向 ai --> sea --> adp
     * 按照数据流向反方向打开实例
     * 按照数据方向attach output
     */
    ret = sample_asr_open_adp(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_asr_open_adp, ret);
        goto out0;
    }

    if (inst->sea_enable) {
        ret = sample_asr_open_sea(inst);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_asr_open_sea, ret);
            goto out1;
        }
    }

    ret = sample_asr_open_ai(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_asr_open_ai, ret);
        goto out2;
    }

    return EXT_SUCCESS;

out2:
    sample_asr_close_sea(inst);
out1:
    sample_asr_close_adp(inst);
out0:
    sample_asr_sys_deinit(inst);
    return ret;
}

static td_void sample_ai_stop_inst(sample_asr_inst *inst)
{
    td_s32 ret;

    if (inst->task_asr != TD_NULL) {
        /* Destory Task to stop ADP instance reading pcm data */
        inst->task_asr_enable = TD_FALSE;
        athread_exit(inst->task_asr);
        inst->task_asr = TD_NULL;
    }

    /* Stop AI instance output pcm data */
    ret = uapi_ai_stop(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_stop, ret);
    }
}

static td_s32 sample_asr_start_inst(sample_asr_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    /* Start AI instance to write data into ADP instance */
    ret = uapi_ai_start(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_start, ret);
        return ret;
    }

    if (inst->sea_enable) {
        ret = uapi_sea_start(inst->h_sea);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_start, ret);
            goto out0;
        }

        sample_gui_aef_add_handle(GUI_AEF_SEA, inst->h_sea);
    }

    inst->task_asr_enable = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_asr";
    ret = athread_create(&inst->task_asr, sample_asr_thread, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_printf("sample asr thread err\n");
        sap_err_log_fun(athread_create, ret);
        goto out1;
    }
    sap_printf("sample asr thread ok\n");

    return EXT_SUCCESS;
out1:
    if (inst->sea_enable) {
        (td_void)uapi_sea_stop(inst->h_sea);
        sample_gui_aef_delete_handle(inst->h_sea);
    }
out0:
    (td_void)uapi_ai_stop(inst->h_ai);
    return ret;
}

static td_void sample_asr_usage(const td_char *name)
{
    sap_printf(" usage: %s sea_enable file.pcm\n", name);
    sap_printf(" sea_enable: {0, 1}\n");
    sap_printf(" file: input stream for asr from file instead of ai\n");
    sap_printf(" ex: sample_asr 0 /user/file.pcm\n");
}

static td_s32 sample_asr_entry(td_char *argv[])
{
    td_s32 ret;
    sample_asr_arg *arg = (sample_asr_arg *)argv;
    sample_asr_inst *inst = sample_asr_get_inst();
    if (inst != TD_NULL) {
        sap_printf("%s is already running\n", argv[0]);
        return EXT_FAILURE;
    }

    inst = sample_asr_alloc_inst();
    if (inst == TD_NULL) {
        sap_printf("sample_asr_alloc_inst failed\n");
        return EXT_FAILURE;
    }

    ret = sample_asr_inst_init(arg, inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_asr_inst_init, ret);
        goto out0;
    }

    ret = sample_asr_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_asr_open_inst, ret);
        goto out1;
    }

    ret = sample_asr_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_asr_start_inst, ret);
        goto out2;
    }

    sap_printf("sample_rate: %u, channels: %u, bit_depth: %u", inst->pcm_format.sample_rate,
        inst->pcm_format.channels, inst->pcm_format.bit_depth);

    return EXT_SUCCESS;
out2:
    sample_asr_close_inst(inst);
out1:
    sample_asr_close_file(inst);
out0:
    sample_asr_free_inst(inst);

    return ret;
}

static td_void sample_asr_exit(td_void)
{
    sample_asr_inst *inst = sample_asr_get_inst();

    if (inst != TD_NULL) {
        sample_ai_stop_inst(inst);
        sample_asr_close_inst(inst);
        sample_asr_close_file(inst);
        sample_asr_free_inst(inst);
    }
}

td_s32 sample_asr(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_asr_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        sample_asr_exit();
        return EXT_SUCCESS;
    } else {
        return sample_asr_entry(argv);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
