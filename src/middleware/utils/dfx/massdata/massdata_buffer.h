/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description:  massdata buffer module.
 */

#ifndef MASSDATA_BUFFER_H
#define MASSDATA_BUFFER_H

#include <stdint.h>
#include "dfx_feature_config.h"
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_DOTTING) && defined(DFX_YES) && \
    (CONFIG_DFX_SUPPORT_MASSDATA_DOTTING == DFX_YES))
#include <stdbool.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_utils_dfx_massdata massdata
 * @ingroup  middleware_utils_dfx_massdata
 * @{
 */
#define CHR_VALUE_NULL              0
#define CHR_INFO_SHFIT_24BIT        24
#define CHR_INFO_SHFIT_16BIT        16
#define CHR_INFO_SHFIT_8BIT         8
#define CHR_VALUE_MASK              0xff
#define CHR_BUFFER_WL_RATIO         2
#define EVENT_DFT_START             0x81
#define MASSDATA_HEAD_MAGIC         0xA8
#define MAX_INFO3_DATA_SIZE         128
#define CHR_PARAM_MAX_LEN           128
#define CHR_PSN_BIT                 26

#define chr_tws_value_group(EVENT, INFO1, INFO2, INFO3) (((uint32_t)((uint32_t)(EVENT) & CHR_VALUE_MASK) << \
                                                        CHR_INFO_SHFIT_24BIT) | \
                                                        ((uint32_t)((uint32_t)(INFO1) & CHR_VALUE_MASK) << \
                                                        CHR_INFO_SHFIT_16BIT) | \
                                                        ((uint32_t)((uint32_t)(INFO2) & CHR_VALUE_MASK) << \
                                                        CHR_INFO_SHFIT_8BIT) | \
                                                        ((INFO3) & CHR_VALUE_MASK))

#define chr_wear_value_group(EVENT, INFO1, INFO2, INFO3) (((uint32_t)((uint32_t)(EVENT) & CHR_VALUE_MASK) << \
                                                         CHR_INFO_SHFIT_24BIT) | \
                                                         ((uint32_t)((uint32_t)(INFO1) & CHR_VALUE_MASK) << \
                                                         CHR_INFO_SHFIT_16BIT) | \
                                                         ((uint32_t)((uint32_t)(INFO2) & CHR_VALUE_MASK) << \
                                                         CHR_INFO_SHFIT_8BIT) | \
                                                         ((INFO3) & CHR_VALUE_MASK))


typedef enum {
    MASS_RET_OK,
    MASS_RET_ERROR_IN_PARAMETERS,
    MASS_RET_ERROR_NOT_ENOUGH_SPACE,
    MASS_RET_ERROR_CORRUPT_SHARED_MEMORY,
    MASS_MEM_COPY_FAIL,
    MASS_OVER_BUFFER_THD,
} mass_data_ret_t;

#define EVENT_ID_BEGIN          943900000
#define EVENT_ID_END            943999999
#define get_event_id(id, info1) (EVENT_ID_BEGIN + (((id) << 8) | (info1)))

typedef enum {
    MASS_EVENT_POINT,
    MASS_ERROR_POINT,
    MASS_EVENT_POINT_EXTEND,
    MASS_ERROR_POINT_EXTEND,
} mass_point_type_t;

/* upload chr type */
typedef enum {
    CHR_DFT = 0x0,
    CHR_UE_INT = 0x01,
    CHR_UE_STR = 0x02,
    CHR_UE_JSON = 0x03,
    CHR_UE_BETA = 0x04,
    CHR_END,
} chr_type_t;

typedef enum {
    MT_START = 0x0,
    MT_VDEC_MANAGER = 0x1,
    MT_VENC_MANAGER = 0x2,
    MT_MEDIA_CHR_AUDIO_MANAGER = 0x7,
    MT_MEDIA_CHR_PLAYER = 0x8,
    MT_MEDIA_CHR_PLAYER_SAMPLE = 0x9,
    MT_MEDIA_CHR_VIDEO_PLAYER_SAMPLE = 0xA,
    MT_MEDIA_CHR_AUDIO_CAPTURE = 0xB,
    MT_MEDIA_CHR_AUDIO_CAPTURE_SAMPLE = 0xC,
    MT_MEDIA_CHR_AUDIO_CAPTURE_JS = 0xD,
    MT_MEDIA_CHR_AUDIO_PLAYER_JS = 0xE,
    MT_GRAPHIC_LOG = 0xF,
    MT_NPU_LOG = 0x10,
	MT_MEDIA_CHR_SMART_VOICE_SREVICE = 0x11,
    MT_PLT_CHR_START = 0x12,
    MT_PLT_CHR_EVENT = MT_PLT_CHR_START,
    MT_PLT_CHR_END = 0x22,
    MT_MAX = 0xFF,
} massdata_event_id;

/**
 * @brief  MASSDATA MEMORY REGIONS
 */
typedef enum {
    MASS_MEMORY_REGION_SECTION_0 = 0, /* !< MASS_MEMORY_REGION_SECTION_0 for security core */
#if CORE_NUMS > 1
    MASS_MEMORY_REGION_SECTION_1,     /* !< MASS_MEMORY_REGION_SECTION_1 for application core */
#endif
    MASS_MEMORY_REGION_MAX_NUMBER,    /* !< MASS_MEMORY_REGION_MAX_NUMBER */
} massdata_memory_region_section_t;

#pragma pack(1)
typedef struct {
    uint8_t type;              // 0-event, 1-error.
    uint8_t event_id;
    uint8_t info1;
    uint8_t info2;
    uint16_t info3;
    uint8_t data_len;
    uint8_t data[CHR_PARAM_MAX_LEN];
} chr_extend_data;
#pragma pack()

/**
 * @brief  Mass data memory control structure for every section
 */
typedef struct {
    volatile uint32_t read;
    volatile uint32_t write;
    volatile uint32_t flow_flag;   /* index if the buffer is overflow write 0-no, 1-yes */
    volatile uint32_t region_len;
    volatile uint32_t region_start;
    volatile uint32_t water_line;
} massdata_memory_section_control_t;

/**
 * @brief  Mass data memory area control structure
 */
typedef struct {
    uint32_t region_num;
    uint32_t mem_len;
    massdata_memory_section_control_t section_control[MASS_MEMORY_REGION_MAX_NUMBER];
} massdata_memory_region_control_t;

/** Macro to get the pointer to the mass data memory control block */
#define MASS_MEMORY_REGION_CONTROL_BLOCK_POINTER ((massdata_memory_region_control_t *)(uintptr_t)MASSDATA_REGION_START)

typedef struct __attribute__ ((__packed__)) {
    uint8_t head_flag;
    uint8_t data_len;
    uint16_t event_id;       /* info0, info1 */
    uint8_t event_info;      /* info2 */
    uint8_t core : 3;
    uint8_t left_right : 1;
    uint8_t reserved : 1;
    uint8_t role : 1;
    uint8_t chr_up_type : 2;
    uint16_t version;
    uint64_t time_stamp : 38;
    uint64_t psn : 25;
    uint64_t time_not_sync_flag : 1;
    uint8_t data[0];
} system_event_s_t, system_error_s_t;

/**
 * @brief  record the system event
 * @param  eid chr info0
 * @param  sub_eid chr info1
 * @param  code mass chr info2
 * @param  sub_code chr info3
 * @return void
 */
void uapi_massdata_record_ue_event(uint8_t event_id, uint8_t info1, uint8_t info2, uint32_t info3);

/**
 * @brief  record the system event.
 * @return void
 */
void uapi_massdata_record_system_event(uint8_t event_id, uint8_t info1, uint8_t info2, uint32_t info3);

/**
 * @brief  record the system long event.
 * @return void
 */
void uapi_massdata_record_system_long_event(uint8_t event_id, uint8_t info1, uint8_t info2,
                                            uint8_t *info3, uint8_t info3_len);

/**
 * @brief  record the system error.
 * @return void
 */
void uapi_massdata_record_system_error(uint8_t event_id, uint8_t info1, uint8_t info2, uint32_t info3);

/**
 * @brief  record the long event.
 * @param eventEnum  (info0 << 16) | (info1 << 8) | info2
 * @param  info3 mass chr info3
 * @param  info3_len the length of info3 (max length is 128 bytes)
 * @return void
 */
void uapi_massdata_record_long_event(uint32_t event_enum, uint8_t *info3, uint8_t info3_len);

void uapi_massdata_set_role(uint8_t role);

/**
 * @brief  init the mass share mem.
 * @return void
 */
void uapi_massdata_buffer_init(massdata_memory_region_section_t sec);

/**
 * @brief  Initialize the mass data memory region and set the alias region for the other cores
 */
void uapi_massdata_memory_region_init(void);

/**
 * @brief  massdata init
 */
void uapi_massdata_init(void);

uint64_t massdata_adapt_get_time(void);

/* 暂时兼容旧接口 */
void massdata_record_system_error(uint8_t event_id, uint8_t info1, uint8_t info2, uint8_t info3);
void massdata_record_system_event(uint8_t event_id, uint8_t info1, uint8_t info2, uint8_t info3);
void massdata_record_system_info_with_extend(chr_extend_data *extend_data);
void log_buffer_record_system_event(uint32_t chr_event);
void log_buffer_record_system_error(uint32_t chr_error);

#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_DOTTING) && (CONFIG_DFX_SUPPORT_MASSDATA_DOTTING == DFX_YES))
bool massdata_get_time_not_sync_flag(void);
void massdata_set_time_not_sync_flag(bool flag);
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_DOTTING */
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* MASSDATA_BUFFER_H */
