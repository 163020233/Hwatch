/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: NV common header file.
 */

#ifndef COMMON_H
#define COMMON_H

#include "stdint.h"
/* 修改此文件后需要先编译A核任意版本生成中间文件application.etypes后才能在编译nv.bin时生效 */
#define BT_CUSTOMIZE_NV_RESERVED 86
#define BT_CUSTOMIZE_CHNL_MAP_LEN 10
/* 基础类型无需在此文件中定义，直接引用即可，对应app.json中的sample0 */

/* 蓝牙地址长度 */
#define BD_ADDR_LEN 6
/* 蓝牙名称长度 */
#define BD_NAME_MAX_LEN       32
/* 蓝牙秘钥索引长度 */
#define BTH_SYS_MASK_LEN  4
/* 蓝牙本端配对地址长度 */
#define BTH_BLE_OWN_ADDR_LEN  24
/* 蓝牙产品信息预留长度 */
#define BTH_PRODUCT_INFORMATION_RESERVE_LEN   32
/* 蓝牙秘钥长度 */
#define BTH_BLE_SMP_DATA_LEN  380
/* 蓝牙预留长度 */
#define BTH_BLE_RESERVE_LEN   128

/* HOST NV配置数据长度 */
#define SLE_CONFIG_SYNC_DATA_SIZE 108
#define SLE_SM_OWN_ADDR_LEN 24
#define SLE_NV_RESERV_LEN   128
#define BR_PRODUCT_INFORMATION_RESERVE_LEN 32
#define BTH_BR_PAIRING_MAX 2
#define BT_LINK_KEY_LEN 16
#define BD_NAME_LEN 32
#define CLS_DEV_LEN 3 /* class of device length */

/* TEE 安全存储 固定key长度 */
#define TEE_SEC_STORAGE_DATA_LEN 134
/* 媒体流音量 固定长度 */
#define MEDIA_VALUE_LEN   (26 * 2)

#define CUSTOMIZE_RESERVE_LEN 31

#define IP_ADDR_LEN 16
/* TEE 安全存储 固定key配置 */
typedef struct {
    uint8_t data[TEE_SEC_STORAGE_DATA_LEN];
} tee_sec_storage_data_t;

/* HOST NV配置结构 */
typedef struct {
    uint8_t bd_addr[BD_ADDR_LEN];
    uint8_t bd_name[BD_NAME_MAX_LEN];
    uint8_t product_type;
    uint8_t sys_mask[BTH_SYS_MASK_LEN];
    uint8_t reserve[BTH_PRODUCT_INFORMATION_RESERVE_LEN];
} bth_product_information_config_t;

/* HOST NV配置结构 */
typedef struct {            /* Keys that indexes by addr */
    uint8_t smp_index;
    uint8_t keys[BTH_BLE_SMP_DATA_LEN];     /* own address, dule-mode? */
} bth_smp_keys_store_nv_stru_t;

typedef struct {            /* Keys that indexes by addr */
    uint8_t reserve[BTH_BLE_RESERVE_LEN];
} bth_ble_nv_reserved_struct_t;

/* SLE HOST NV配置结构 */
typedef struct {
    uint8_t sle_addr[BD_ADDR_LEN];
    uint8_t sle_name[BD_NAME_MAX_LEN];
    uint8_t sys_mask[BTH_SYS_MASK_LEN];
    uint8_t reserve[BTH_PRODUCT_INFORMATION_RESERVE_LEN];
} sle_product_data_config_stru_t;

/* BR HOST NV配置结构 */
typedef struct {
    uint8_t product_type[CLS_DEV_LEN];
    uint8_t bd_addr[BD_ADDR_LEN];
    uint8_t bd_name[BD_NAME_MAX_LEN];
    uint8_t sys_mask[BTH_SYS_MASK_LEN];
    uint8_t reserve[BR_PRODUCT_INFORMATION_RESERVE_LEN];
} br_product_information_config_t;

/* 已配对设备持久化结构 */
typedef struct {
    uint8_t peer_addr[BD_ADDR_LEN];
    uint8_t key_type; /* 密钥类型，参考 { @ref bt_link_key_type_t } */
    uint8_t link_key[BT_LINK_KEY_LEN];
    uint8_t name[BD_NAME_LEN];
    int32_t class_of_device;
} br_paired_dev_info_t;

typedef struct {            /* Keys that indexes by addr */
    uint8_t index;
    br_paired_dev_info_t devs[BTH_BR_PAIRING_MAX];
} br_paired_dev_strore_stru_t;

typedef struct {
    uint8_t name[BD_NAME_MAX_LEN];
} ux_bt_name_t;

typedef struct {
    uint8_t dns_tc[IP_ADDR_LEN];
    uint8_t dns_ali[IP_ADDR_LEN];
    uint8_t dns_cnn[IP_ADDR_LEN];
} net_dns_addr_t;

typedef struct {
    uint16_t config_nv_1;
    uint16_t config_nv_2;
    uint16_t config_nv_3;
    uint16_t config_nv_4;
    uint16_t config_nv_5[5];
    uint16_t config_nv_6;
    uint16_t config_nv_7;
    uint16_t config_nv_8;
    uint16_t config_nv_9;
    uint8_t config_nv_10;
    int8_t config_nv_11;
    int8_t config_nv_12;
    uint8_t flag;
} bt_nv_cali_info_type_t;

typedef struct {
    uint8_t max_tx_power_level;
    uint8_t reserved[CUSTOMIZE_RESERVE_LEN];  // 预留31
} bt_nv_customize_param_t;


/* 3A prams */
#define NV_AUDIO_VQE_PARAM_SIZE 53
typedef struct {
    uint32_t size;
    uint32_t data[NV_AUDIO_VQE_PARAM_SIZE];
} audio_vqe_param_t;

/* SWS aef prams */
#define NV_AUDIO_SWS_PARAM_SIZE     162
#define NV_AUDIO_HVS_PARAM_SIZE     604
typedef struct {
    uint32_t size;
    uint16_t data[NV_AUDIO_SWS_PARAM_SIZE];
} audio_sws_param_t;

typedef struct {
    uint32_t size;
    uint16_t data[NV_AUDIO_HVS_PARAM_SIZE];
} audio_hvs_param_t;

/* AHE prams */
#define NV_AUDIO_USER_PARAM_SIZE    56
#define NV_NR_PARAM_SIZE            8
#define NV_AUDIO_AGC_PARAM_SIZE     8
typedef struct {
    uint32_t size;
    uint32_t data[NV_AUDIO_USER_PARAM_SIZE];
} audio_ahe_user_param_t;

typedef struct {
    uint32_t size;
    uint32_t data[NV_NR_PARAM_SIZE];
} nr_mic4stream_param_t;

typedef struct {
    uint32_t size;
    uint32_t data[NV_AUDIO_AGC_PARAM_SIZE];
} audio_agc_param_t;

/* sys language id */
typedef struct {
    uint8_t language_id;
} sys_language_id_t;

/* sys product info */
#define NV_SYS_PRODUCT_INFO_NAME_LEN 32
typedef struct {
    uint8_t size;
    uint8_t name[NV_SYS_PRODUCT_INFO_NAME_LEN];
} sys_product_info_t;

typedef struct {
    uint16_t value[MEDIA_VALUE_LEN];
} audio_stream_common_t;

#define ESIM_BIND_EID_LENGTH 35
#define ESIM_BIND_SHARE_KEY_LENGTH 67
#define IMEI_LENGTH 31
#define N_LENGTH 600
#define D_LENGTH 600
#define E_LENGTH 20
typedef struct {
    uint8_t valid;
    uint8_t imei[IMEI_LENGTH];
    uint8_t eid[ESIM_BIND_EID_LENGTH];
    uint8_t share_key[ESIM_BIND_SHARE_KEY_LENGTH];
    uint8_t esim_n[N_LENGTH];
    uint8_t esim_d[D_LENGTH];
    uint8_t esim_e[E_LENGTH];
} esim_bind_info_t;

#endif /* COMMON_H */
