/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: NV - key_id.
 */

#ifndef KEY_ID_H
#define KEY_ID_H

#define REGION_KEY_NUMS                         0x1000 /* 每个区域容纳key_id的个数 */

/* Please use the format of NV_ID_XXX */

/* system factory key_id [0x0001,0x1000) */
#define BTC_CONFIG_NV_INFO_ID                   0x000B
#define BTH_BLE_NV_RESERVED_ID                  0x000E
#define NV_ID_BT_FACTORY_ADDR                   0x0209

/* user factory key_id [0x1000,0x2000) */
#define NV_ID_BTSRV_XOTRIM0_CONFIG              0x1008
#define NV_ID_UX_BT_NAME                        0x1208
#define NV_ID_LANGUAGE                          0x1308

/* system normal key_id [0x2000,0x3000) */
#define NV_ID_OFFLINELOG_ENBALE_FLAG            0x2002 /* Offline log enable flag */
#define NV_ID_TIMEZONE_ID                       0x2003

#define NV_ID_SYS_LANGUAGE_ID                   0x2105  /* sys_language_id */
#define NV_ID_SYS_PRODUCT_INFO                  0x2106  /* sys_product_info */
#define NV_ID_SYS_ESIM_BIND_INFO                0x2107  /* esim bind info */

/* HCD(HiSpark Cloud Diag) key_id [0x2200,0x2300) */
#define NV_ID_HCD_LOG_ENABLE_FLAG               0x2201 /* HCD log report enable flag */
#define NV_ID_HCD_METRICS_ENABLE_FLAG           0x2202 /* HCD metrics report enable flag */

/* dial market key_id [0x2500,0x2600) */
#define NV_ID_SECURE_DIAL                       0x2500 /* secure dial */

// Use a special keyId to store the bitmap (before the key range)
#define BITMAP_KEY_ID                           0x26FF
/* ace cipher key_id [0x2700,0x2900) */
#define NV_ID_ACE_CIPHER                        0x2700 /* ace cipher key id */

/* user normal key_id [0x3000,0x4000) */
#define NV_ID_AUDIO_VQE_PARAM                   0x320C /* 3A params for VOIP scene, audio_vqe_param_t */
#define NV_ID_AUDIO_VQE_KWS_PARAM               0x320D /* 3A params for KWS scene, audio_vqe_param_t */
#define NV_ID_AUDIO_AEF_SWS_PARAM               0x320E /* 3A params for SWS scene, audio_sws_param_t */
#define NV_ID_AUDIO_AEF_HVS_PARAM               0x320F /* 3A params for HVS scene, audio_hvs_param_t */
#define NV_ID_AUDIO_AHE_USR_PARAM               0x3210 /* 2A params for user param, audio_ahe_user_param_t */
#define NV_ID_AUDIO_NR_OFF_MIC4STREAM_PARA      0x3211 /* 2A params for NR param in off anr mode, nr_mic4stream_param_t */
#define NV_ID_AUDIO_NR_ONLY_MIC4STREAM_PARA     0x3212 /* 2A params for NR param in normal anr mode, nr_mic4stream_param_t */
#define NV_ID_AUDIO_NR_WNR_MIC4STREAM_PARA      0x3213 /* 2A params for NR param in enhance anr mode, nr_mic4stream_param_t */
#define NV_ID_AUDIO_NR_OFF_MIC4STREAM_AGC_PARA  0x3214 /* 2A params for agc param in off anr mode, audio_agc_param_t */
#define NV_ID_AUDIO_NR_ONLY_MIC4STREAM_AGC_PARA 0x3215 /* 2A params for agc param in normal anr mode, audio_agc_param_t */
#define NV_ID_AUDIO_NR_WNR_MIC4STREAM_AGC_PARA  0x3216 /* 2A params for agc param in enhance anr mode, audio_agc_param_t */
#define NV_ID_AUDIO_ANR_MODE_PARA               0x3217 /* 2A params for anr mode, audio_anr_mode_param_t */

#define NV_ID_BT_CUSTOMIZE_PARAM                0x3300 /* BT客户定制化参数 */

/* reserved key_id [0x4000,0x6FFF) */
#define NV_ID_HAID_PARAM                        0x4228
#define NV_ID_HAID_HW_PARAM                     0x4229
#define NV_ID_HAID_MEDICAL_PARAM                0x422A

#define NV_ID_TIMESTAMP_PARA                    0x422F
#define NV_ID_DEV_STATISTIC                     0x4230
#define NV_ID_FTM_CONFIG                        0x4231
#define NV_ID_DEV_BUTTON_CONFIG                 0x4232
#define NV_ID_SCENE_MODE_STATUS                 0x4233
#define NV_ID_UX_RESET_CONFIG                   0x4234
#define NV_ID_UX_POWER_OFF_REASON               0x4235

/* [0x7000 ~ 0x7FFF]: Media-related keys */
#define NV_ID_MEDIA_STREAM_VOLUME               0x7000
#define NV_ID_MEDIA_STREAM_MIX_WEIGHT           0x7001

#define NV_ID_NET_DNS_ADDR                      0x8000

/* reserved key_id [0x8000,0xFFFF) */
#define NV_ID_RESTORE_ENABLE                    0xA000 /* NV restore factory region flag */

#endif /* __KEY_ID_H__ */
