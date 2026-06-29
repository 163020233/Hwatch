/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: audio diag
 * Author: audio
 * Create: 2025-10-17
 */

#ifdef CONFIG_NV_FEATURE_SUPPORT
#include "nv.h"
#endif
#ifdef SUPPORT_MEDIA_AUDIO_MANAGER
#include "audio_manager_service.h"
#endif
#include "debug_print.h"
#include "diag_audio.h"

#define GET_SEE_PARAM_FIRST_POS 0
#define DEBUGKITS_READ_PARAM_TAG 0xff
#define PARSE_EFFECT_PARAM_FLAG_POS 0
#define NV_NR_PARAM_SIZE            8
#define NV_AUDIO_USER_PARAM_SIZE    56
#define NV_AUDIO_AGC_PARAM_SIZE     8
// 参考 uapi_sea_afe_type UAPI_SEA_AFE_SEE 枚举
#define audio_unused(var) (void)(var)
#define NV_AUDIO_NR_MODE_MAX 3
#define AUDIO_PARAM_USER 0
#define AUDIO_PARAM_NR 1
#define AUDIO_PARAM_AGC 2

/*
 * Debugkit menu点击通知 读取/配置 3A算法参数
 *
 * 当点击Read时，读取当前3A算法参数
 * 当点击Write时，将界面中的3A算法参数配置下去，并使能到算法中
 */
#ifdef SUPPORT_MEDIA_AUDIO_MANAGER

static uint16_t g_nr_nv_id_map[NV_AUDIO_NR_MODE_MAX] = {
    NV_ID_AUDIO_NR_OFF_MIC4STREAM_PARA,
    NV_ID_AUDIO_NR_ONLY_MIC4STREAM_PARA,
    NV_ID_AUDIO_NR_WNR_MIC4STREAM_PARA
};

static uint16_t g_agc_nv_id_map[NV_AUDIO_NR_MODE_MAX] = {
    NV_ID_AUDIO_NR_OFF_MIC4STREAM_AGC_PARA,
    NV_ID_AUDIO_NR_ONLY_MIC4STREAM_AGC_PARA,
    NV_ID_AUDIO_NR_WNR_MIC4STREAM_AGC_PARA
};

errcode_t diag_see_param_trans(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    int32_t ret;
    uint8_t *param = (uint8_t *)cmd_param;

    if (cmd_param_size != NV_AUDIO_VQE_PARAM_SIZE * sizeof(uint32_t)) {
        PRINT("[audio][diag] cmd_param_size invalid\r\n");
        return ERRCODE_FAIL;
    }

    if (param[GET_SEE_PARAM_FIRST_POS] != DEBUGKITS_READ_PARAM_TAG) {
        /* set param , this func return 0 when success */
        ret = AudioManagerSetSeaParam(AUDIO_SEA_AFE_SEE, (uint8_t *)param, cmd_param_size);
        if (ret != 0) {
            PRINT("[audio][diag] set param fail\r\n");
            return ERRCODE_FAIL;
        }
    } else {
        /* get param , this func return 0 when success */
        ret = AudioManagerGetSeaParam(AUDIO_SEA_AFE_SEE, (uint8_t *)param, cmd_param_size);
        if (ret != 0) {
            PRINT("[audio][diag] get param fail\r\n");
            return ERRCODE_FAIL;
        }
    }

    return uapi_diag_report_packet(cmd_id, option, (uint8_t *)cmd_param, (uint16_t)cmd_param_size, true);
}

static errcode_t audio_manager_save_sea_param(uint8_t *sea_param, uint16_t len)
{
    if (sea_param == NULL || len != sizeof(audio_vqe_param_t)) {
        PRINT("[audio_manager_save_sea_param]sea_param null or len invalid");
        return ERRCODE_INVALID_PARAM;
    }

#ifdef CONFIG_NV_FEATURE_SUPPORT
    errcode_t nv_op = uapi_nv_write((uint16_t)NV_ID_AUDIO_VQE_PARAM, sea_param, len);
    if (nv_op != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#endif
    return ERRCODE_SUCC;
}

/* debugkit menu点击Save保存所有3A参数 */
errcode_t diag_see_param_save(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    audio_vqe_param_t *nv_data = (audio_vqe_param_t *)malloc(sizeof(audio_vqe_param_t));
    if (nv_data == NULL) {
        PRINT("[audio][diag] malloc fail\r\n");
        return ERRCODE_MALLOC;
    }
    (void)memset_s(nv_data, sizeof(audio_vqe_param_t), 0, sizeof(audio_vqe_param_t));

    nv_data->size = NV_AUDIO_VQE_PARAM_SIZE;
    int32_t ret = AudioManagerGetSeaParam(AUDIO_SEA_AFE_SEE, (uint8_t *)nv_data->data, NV_AUDIO_VQE_PARAM_SIZE * sizeof(uint32_t));
    if (ret != 0) {
        free(nv_data);
        PRINT("[audio][diag] get param fail\r\n");
        return ERRCODE_FAIL;
    }

    errcode_t nv_op = audio_manager_save_sea_param((uint8_t *)nv_data, (uint16_t)sizeof(audio_vqe_param_t));
    if (nv_op != ERRCODE_SUCC) {
        free(nv_data);
        PRINT("[audio][diag] save param nv fail\r\n");
        return ERRCODE_FAIL;
    }
    free(nv_data);

    errcode_t op = uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ret, sizeof(uint8_t), true);
    if (op != ERRCODE_SUCC) {
        PRINT("[audio][diag] read rpt fail %u\r\n", op);
    }

    audio_unused(cmd_param);
    audio_unused(cmd_param_size);
    return op;
}

errcode_t diag_ahe_param_trans(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    errcode_t ret;
    diag_nr_cmd_param_struct *param = (diag_nr_cmd_param_struct *)cmd_param;
    uint32_t sea_cmd_id;

    if (cmd_param_size < sizeof(diag_nr_cmd_param_struct)) {
        PRINT("[audio][diag] cmd_param_size invalid\r\n");
        return ERRCODE_FAIL;
    }
    uint32_t param_len = 0;
    if (param->data_type == AUDIO_PARAM_USER) {
        sea_cmd_id = AUDIO_SEA_AFE_SEE_RW_COMMON_PARAM_CMD;
        param_len = NV_AUDIO_USER_PARAM_SIZE * sizeof(uint32_t);
    } else if (param->data_type == AUDIO_PARAM_NR) {
        sea_cmd_id = AUDIO_SEA_AFE_SEE_RW_NR_PARAM_CMD;
        param_len = NV_NR_PARAM_SIZE * sizeof(uint32_t);
    } else if (param->data_type == AUDIO_PARAM_AGC) {
        sea_cmd_id = AUDIO_SEA_AFE_SEE_RW_AGC_PARAM_CMD;
        param_len = NV_AUDIO_AGC_PARAM_SIZE * sizeof(uint32_t);
    } else {
        PRINT("[audio][diag] cmd_param_size invalid\r\n");
        return ERRCODE_FAIL;
    }
    
    if (param->head.mod_id != DEBUGKITS_READ_PARAM_TAG) {
        /* set param , this func return 0 when success */
        ret = AudioManagerSetSeaParam(sea_cmd_id, (uint8_t *)param->params, param_len);
        if (ret != ERRCODE_SUCC) {
            PRINT("[audio][diag] set param fail\r\n");
            return ERRCODE_FAIL;
        }
    } else {
        /* get param , this func return 0 when success */
        ret = AudioManagerGetSeaParam(sea_cmd_id, (uint8_t *)param->params, param_len);
        if (ret != ERRCODE_SUCC) {
            PRINT("[audio][diag] get param fail\r\n");
            return ERRCODE_FAIL;
        }
    }

    return uapi_diag_report_packet(cmd_id, option, (uint8_t *)cmd_param, (uint16_t)cmd_param_size, true);
}

static errcode_t diag_ahe_usr_param_save(diag_nr_cmd_param_struct *param)
{
    audio_ahe_user_param_t *nv_data = (audio_ahe_user_param_t *)malloc(sizeof(audio_ahe_user_param_t));
    if (nv_data == NULL) {
        PRINT("[audio][diag] malloc audio_ahe_user_param_t error\r\n");
        return ERRCODE_FAIL;
    }

    (void)memset_s(nv_data, sizeof(audio_ahe_user_param_t), 0, sizeof(audio_ahe_user_param_t));
    nv_data->size = NV_AUDIO_USER_PARAM_SIZE;

    int32_t ret = AudioManagerGetSeaParam(AUDIO_SEA_AFE_SEE_RW_COMMON_PARAM_CMD, (uint8_t *)nv_data->data,
        NV_AUDIO_USER_PARAM_SIZE * sizeof(uint32_t));
    if (ret != ERRCODE_SUCC) {
        PRINT("[audio][diag] AudioManagerGetSeaParam fail\r\n");
        free(nv_data);
        return ret;
    }

    uint16_t nv_id = NV_ID_AUDIO_AHE_USR_PARAM;
    errcode_t nv_ret = uapi_nv_write(nv_id, (uint8_t *)nv_data, sizeof(audio_ahe_user_param_t));
    if (nv_ret != ERRCODE_SUCC) {
        PRINT("[audio][diag] uapi_nv_write audio user param fail\r\n");
        free(nv_data);
        return nv_ret;
    }

    free(nv_data);
    return nv_ret;
}

static errcode_t diag_ahe_nr_param_save(diag_nr_cmd_param_struct *param)
{
    if (param->head.mode >= NV_AUDIO_NR_MODE_MAX) {
        PRINT("[audio][diag] nr mode invalid\r\n");
        return ERRCODE_FAIL;
    }

    nr_mic4stream_param_t *nv_data = (nr_mic4stream_param_t *)malloc(sizeof(nr_mic4stream_param_t));
    if (nv_data == NULL) {
        PRINT("[audio][diag] malloc nr_mic4stream_param_t error\r\n");
        return ERRCODE_FAIL;
    }

    (void)memset_s(nv_data, sizeof(nr_mic4stream_param_t), 0, sizeof(nr_mic4stream_param_t));
    nv_data->size = NV_NR_PARAM_SIZE;

    int32_t ret = AudioManagerGetSeaParam(AUDIO_SEA_AFE_SEE_RW_NR_PARAM_CMD, (uint8_t *)nv_data->data,
        NV_NR_PARAM_SIZE * sizeof(uint32_t));
    if (ret != ERRCODE_SUCC) {
        PRINT("[audio][diag] AudioManagerGetSeaParam fail\r\n");
        free(nv_data);
        return ret;
    }

    uint16_t nv_id = g_nr_nv_id_map[param->head.mode];
    errcode_t nv_ret = uapi_nv_write(nv_id, (uint8_t *)nv_data, sizeof(nr_mic4stream_param_t));
    if (nv_ret != ERRCODE_SUCC) {
        PRINT("[audio][diag] uapi_nv_write audio nr param fail\r\n");
        free(nv_data);
        return nv_ret;
    }

    free(nv_data);
    return nv_ret;
}

static errcode_t diag_ahe_agc_param_save(diag_nr_cmd_param_struct *param)
{
    if (param->head.mode >= NV_AUDIO_NR_MODE_MAX) {
        PRINT("[audio][diag] nr mode invalid\r\n");
        return ERRCODE_FAIL;
    }

    audio_agc_param_t *nv_data = (audio_agc_param_t *)malloc(sizeof(audio_agc_param_t));
    if (nv_data == NULL) {
        PRINT("[audio][diag] malloc audio_agc_param_t error\r\n");
        return ERRCODE_FAIL;
    }

    (void)memset_s(nv_data, sizeof(audio_agc_param_t), 0, sizeof(audio_agc_param_t));
    nv_data->size = NV_AUDIO_AGC_PARAM_SIZE;

    int32_t ret = AudioManagerGetSeaParam(AUDIO_SEA_AFE_SEE_RW_AGC_PARAM_CMD, (uint8_t *)nv_data->data,
        NV_AUDIO_AGC_PARAM_SIZE * sizeof(uint32_t));
    if (ret != ERRCODE_SUCC) {
        PRINT("[audio][diag] AudioManagerGetSeaParam fail\r\n");
        free(nv_data);
        return ret;
    }

    uint16_t nv_id = g_agc_nv_id_map[param->head.mode];
    errcode_t nv_ret = uapi_nv_write(nv_id, (uint8_t *)nv_data, sizeof(audio_agc_param_t));
    if (nv_ret != ERRCODE_SUCC) {
        PRINT("[audio][diag] uapi_nv_write audio agc param fail\r\n");
        free(nv_data);
        return nv_ret;
    }

    free(nv_data);
    return nv_ret;
}

errcode_t diag_ahe_param_save(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    errcode_t ret;
    diag_nr_cmd_param_struct *param = (diag_nr_cmd_param_struct *)cmd_param;
    uint32_t sea_cmd_id;

    if (cmd_param_size < sizeof(diag_nr_cmd_param_struct)) {
        PRINT("[audio][diag] cmd_param_size invalid\r\n");
        return ERRCODE_FAIL;
    }

    switch (param->data_type) {
        case AUDIO_PARAM_USER:
            ret = diag_ahe_usr_param_save(param);
            break;
        case AUDIO_PARAM_NR:
            ret = diag_ahe_nr_param_save(param);
            break;
        case AUDIO_PARAM_AGC:
            ret = diag_ahe_agc_param_save(param);
            break;
        default:
            PRINT("[audio][diag] cmd_param_size invalid\r\n");
            return ERRCODE_FAIL;
    }

    if (ret != ERRCODE_SUCC) {
        PRINT("[audio][diag] diag_ahe_param_save failed!\r\n");
        return ERRCODE_FAIL;
    }
    return uapi_diag_report_packet(cmd_id, option, (uint8_t *)cmd_param, (uint16_t)cmd_param_size, true);
}

errcode_t diag_ahe_mode_trans(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    errcode_t ret;
    uint8_t nv_data = 0;
    uint16_t len = 0;
    diag_nr_cmd_param_struct *param = (diag_nr_cmd_param_struct *)cmd_param;
    if (param->head.mod_id != DEBUGKITS_READ_PARAM_TAG) {
        if (param->head.mode >= NV_AUDIO_NR_MODE_MAX) {
            PRINT("[audio][diag] nr mode invalid\r\n");
            return ERRCODE_FAIL;
        }
        nv_data = param->head.mode;
        errcode_t nv_ret = uapi_nv_write(NV_ID_AUDIO_ANR_MODE_PARA, (const uint8_t*)&nv_data, sizeof(uint8_t));
        if (nv_ret != ERRCODE_SUCC) {
            PRINT("[audio][diag] uapi_nv_write audio anr mode fail\r\n");
            return nv_ret;
        }
    } else {
        errcode_t nv_ret = uapi_nv_read(NV_ID_AUDIO_ANR_MODE_PARA, sizeof(uint8_t), &len, &nv_data);
        if (nv_ret != ERRCODE_SUCC || len != sizeof(uint8_t)) {
            PRINT("[audio][diag] uapi_nv_read audio anr mode fail\r\n");
            return nv_ret;
        }
        param->head.mode = nv_data;
    }
    return uapi_diag_report_packet(cmd_id, option, (uint8_t *)cmd_param, cmd_param_size, true);
}

/* debugkit menu点击read/write用于获取或设置AEF参数 */
errcode_t diag_aef_param_trans(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    int32_t ret;
    uint8_t *param = (uint8_t *)cmd_param;
    if (cmd_param_size < NV_AUDIO_SWS_PARAM_SIZE) {
        PRINT("[audio][diag][aef] cmd_param_size invalid\r\n");
        return ERRCODE_FAIL;
    }

    if (param[GET_SEE_PARAM_FIRST_POS] != DEBUGKITS_READ_PARAM_TAG) {
        /* set param , this func return 0 when success */
        ret = AudioManagerSetAefParam(SND_PORT_AEF_TYPE, (uint8_t *)param, NV_AUDIO_SWS_PARAM_SIZE);
        if (ret != 0) {
            PRINT("[audio][diag][aef] set param fail, errcode:%d\r\n", ret);
            return ERRCODE_FAIL;
        } else {
            PRINT("[audio][diag][aef] set param success !\r\n");
        }
    } else {
        /* get param , this func return 0 when success */
        ret = AudioManagerGetAefParam(SND_PORT_AEF_TYPE, (uint8_t *)param, NV_AUDIO_SWS_PARAM_SIZE);
        if (ret != 0) {
            PRINT("[audio][diag][aef] get param fail, errcode:%d\r\n", ret);
            return ERRCODE_FAIL;
        } else {
            PRINT("[audio][diag][aef] get param success !\r\n");
        }
    }
    return uapi_diag_report_packet(cmd_id, option, (uint8_t *)cmd_param, NV_AUDIO_SWS_PARAM_SIZE, true);
}

/* debugkit menu点击read/write用于获取或设置HVS参数 */
errcode_t diag_hvs_param_trans(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    int32_t ret;
    uint8_t *param = (uint8_t *)cmd_param;
    if (cmd_param_size < NV_AUDIO_HVS_PARAM_SIZE) {
        PRINT("[audio][diag][aef] cmd_param_size invalid\r\n");
        return ERRCODE_FAIL;
    }

    if (param[GET_SEE_PARAM_FIRST_POS] != DEBUGKITS_READ_PARAM_TAG) {
        /* set param , this func return 0 when success */
        ret = AudioManagerSetAefParam(SND_PORT_AEF_TYPE, (uint8_t *)param, NV_AUDIO_HVS_PARAM_SIZE);
        if (ret != 0) {
            PRINT("[audio][diag][aef] set param fail, errcode:%d\r\n", ret);
            return ERRCODE_FAIL;
        } else {
            PRINT("[audio][diag][aef] set param success !\r\n");
        }
    } else {
        /* get param , this func return 0 when success */
        ret = AudioManagerGetAefParam(SND_PORT_AEF_TYPE, (uint8_t *)param, NV_AUDIO_HVS_PARAM_SIZE);
        if (ret != 0) {
            PRINT("[audio][diag][aef] get param fail, errcode:%d\r\n", ret);
            return ERRCODE_FAIL;
        } else {
            PRINT("[audio][diag][aef] get param success !\r\n");
        }
    }
    return uapi_diag_report_packet(cmd_id, option, (uint8_t *)cmd_param, NV_AUDIO_HVS_PARAM_SIZE, true);
}

static errcode_t audio_manager_save_aef_param(uint8_t *aef_param, uint16_t len)
{
    if (aef_param == NULL || len != sizeof(audio_sws_param_t)) {
        PRINT("[audio_manager_save_aef_param]sea_param null or len invalid");
        return ERRCODE_INVALID_PARAM;
    }

#ifdef CONFIG_NV_FEATURE_SUPPORT
    errcode_t nv_op = uapi_nv_write((uint16_t)NV_ID_AUDIO_AEF_SWS_PARAM, aef_param, len);
    if (nv_op != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#endif
    return ERRCODE_SUCC;
}

static errcode_t audio_manager_save_hvs_param(uint8_t *aef_param, uint16_t len)
{
    if (aef_param == NULL || len != sizeof(audio_hvs_param_t)) {
        PRINT("[audio_manager_save_aef_param]sea_param null or len invalid");
        return ERRCODE_INVALID_PARAM;
    }

#ifdef CONFIG_NV_FEATURE_SUPPORT
    errcode_t nv_op = uapi_nv_write((uint16_t)NV_ID_AUDIO_AEF_HVS_PARAM, aef_param, len);
    if (nv_op != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#endif
    return ERRCODE_SUCC;
}

/* debugkit menu点击Save保存所有AEF参数 */
errcode_t diag_sws_param_save(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    PRINT("[audio][diag][aef] diag_sws_param_save comes\r\n");
    audio_sws_param_t *nv_data = (audio_sws_param_t *)malloc(sizeof(audio_sws_param_t));
    if (nv_data == NULL) {
        PRINT("[audio][diag][aef] nv malloc fail\r\n");
        return ERRCODE_MALLOC;
    }
    (void)memset_s(nv_data, sizeof(audio_sws_param_t), 0, sizeof(audio_sws_param_t));

    nv_data->size = NV_AUDIO_SWS_PARAM_SIZE;
    int32_t ret = AudioManagerGetAefParam(SND_PORT_AEF_TYPE, (uint8_t *)nv_data->data,
        NV_AUDIO_SWS_PARAM_SIZE * sizeof(uint8_t));
    if (ret != 0) {
        free(nv_data);
        PRINT("[audio][diag][save] get aef param fail\r\n");
        return ERRCODE_FAIL;
    }

    errcode_t nv_op = audio_manager_save_aef_param((uint8_t *)nv_data, (uint16_t)sizeof(audio_sws_param_t));
    if (nv_op != ERRCODE_SUCC) {
        free(nv_data);
        PRINT("[audio][diag] save aef param nv fail\r\n");
        return ERRCODE_FAIL;
    }
    free(nv_data);

    errcode_t op = uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ret, sizeof(uint8_t), true);
    if (op != ERRCODE_SUCC) {
        PRINT("[audio][diag] read rpt fail %u\r\n", op);
    } else {
        PRINT("[audio][diag][aef] read rpt success\r\n");
    }
    audio_unused(cmd_param);
    audio_unused(cmd_param_size);
    return op;
}

errcode_t diag_hvs_param_save(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    audio_hvs_param_t *nv_data = (audio_hvs_param_t *)malloc(sizeof(audio_hvs_param_t));
    if (nv_data == NULL) {
        PRINT("[audio][diag][aef] nv malloc fail\r\n");
        return ERRCODE_MALLOC;
    }

    (void)memset_s(nv_data, sizeof(audio_hvs_param_t), 0, sizeof(audio_hvs_param_t));

    nv_data->size = NV_AUDIO_HVS_PARAM_SIZE;
    int32_t ret = AudioManagerGetAefParam(SND_PORT_AEF_TYPE, (uint8_t *)nv_data->data,
        NV_AUDIO_HVS_PARAM_SIZE * sizeof(uint8_t));
    if (ret != 0) {
        free(nv_data);
        PRINT("[audio][diag][save] get aef param fail\r\n");
        return ERRCODE_FAIL;
    }

    errcode_t nv_op = audio_manager_save_hvs_param((uint8_t *)nv_data, (uint16_t)sizeof(audio_hvs_param_t));
    if (nv_op != ERRCODE_SUCC) {
        free(nv_data);
        PRINT("[audio][diag] save aef param nv fail\r\n");
        return ERRCODE_FAIL;
    }
    free(nv_data);

    errcode_t op = uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ret, sizeof(uint8_t), true);
    if (op != ERRCODE_SUCC) {
        PRINT("[audio][diag] read rpt fail %u\r\n", op);
    } else {
        PRINT("[audio][diag][aef] read rpt success\r\n");
    }
    audio_unused(cmd_param);
    audio_unused(cmd_param_size);
    return op;
}

#endif /* SUPPORT_MEDIA_AUDIO_MANAGER */