/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2006-2024. All rights reserved.
 * Description: vfmw config
 * Author: AuthorNameMagicTag
 * Create: 2006-01-01
 */

#ifndef VFMW_CONFIG_H
#define VFMW_CONFIG_H

#include "memory_config.h"

/* product memory config */
#define VFMW_CHECK_MEM_SUPPORT

#ifdef VFMW_OVER_SPEC_SUPPORT
// vfmw manage psram memory pool
#define VFMW_MANAGE_MEM_SUPPORT
#define VFMW_MEM_ADDR    (PSRAM_NPU_APP_HEAP_START)
#define VFMW_MEM_LEN     (PSRAM_NPU_APP_HEAP_LENGTH + PSRAM_GUI_MEM_POOL_SIZE + PSRAM_DISP_VAU_NODE_LENGTH)
#elif defined VFMW_MANAGE_MEM_SUPPORT
#define VFMW_MEM_ADDR    (PSRAM_GUI_MEM_POOL_ADDR)
#define VFMW_MEM_LEN     (PSRAM_GUI_MEM_POOL_SIZE)
#else
#define VFMW_PSRAM_CACHE_SUPPORT
#define VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
#define VFMW_MEM_ADDR    (0xFFFFFFFF)
#define VFMW_MEM_LEN     (0x0)
#endif

/* vfmw config */
#define VFMW_MAX_CHAN_NUM      2
#define VFMW_H264_SUPPORT
#define H264_MIN_PIX_WIDTH     80
#define H264_MIN_PIX_HEIGHT    96
#define H264_B_FRAME_SUPPORT
#ifdef VFMW_OVER_SPEC_SUPPORT
#define H264_MAX_PIX_WIDTH     3840
#define H264_MAX_PIX_HEIGHT    2160
#define H264_MAX_REF_NUM       16
#define H264_MAX_SLICE_NUM     1024
#define H264_FIELD_SUPPORT
#define MAX_RAW_NUM            1024
#define MAX_SEG_NUM            1024
#else
#define H264_MAX_REF_NUM       2
#define H264_MAX_PIX_WIDTH     1280
#define H264_MAX_PIX_HEIGHT    1280 /* 1280: 640x2 support spliced stream:sp420+A8 */
#define H264_MAX_SLICE_NUM     100
#define MAX_RAW_NUM            400
#define MAX_SEG_NUM            400
#endif
#define MJPEG_MAX_PIX_WIDTH    600
#define MJPEG_MAX_PIX_HEIGHT   600

#define VFMW_VPP_SUPPORT

#define SCD_TIME_OUT           4000
#define VDH_TIME_OUT           10000
#define MAX_DPS_NUM            2
#define VFMW_FSP_MAX_FRAME_NUM 32

#define VDH_DEC_SUPPORT
#ifndef VDEC_NO_DEBUG_LOG_SUPPORT
#define VFMW_DPRINT_SUPPORT
#endif
#define VFMW_STREAM_SUPPORT
#define VFMW_LOCAL_SUPPORT
#define VFMW_USER_SUPPORT

// scd
#define RAW_MEM_LESS
#define IP_UNID_SUPPORT

#ifdef VDEC_LOW_POWER_SUPPORT
// low power
/* If there are multiple sets of registers, this feature needs to be redesigned. */
#define VFMW_LOW_POWER_SUPPORT
#endif

#endif
