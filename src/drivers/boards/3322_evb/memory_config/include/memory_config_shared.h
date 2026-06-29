/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description:  Shared memory configurations
 * Author: @CompanyNameTag
 * Create:  2024-11-09
 */

#ifndef MEMORY_CONFIG_SHARED_H
#define MEMORY_CONFIG_SHARED_H

#define SHARED_MEM_START                  0x83000000
#define SHARED_MEM_LENGTH                 0x20000  // 128K
#define SHARED_MEM_END                    (SHARED_MEM_START + SHARED_MEM_LENGTH)

/* Romboot info 512bytes */
#define ROMBOOT_INFO_REGION_START         (SHARED_MEM_START)
#define ROMBOOT_INFO_REGION_LENGTH        (0x200)

#define IPC_REGION_START                  (ROMBOOT_INFO_REGION_START + ROMBOOT_INFO_REGION_LENGTH)
// CH0控制通道. TX, RX大小统一为1k byte 0xC共享内存头大小
#define CTL_CHL_LEN                       (0x400 + 0xC)
// CH1数据通道. 通用为3K
#define DATA_CHL_LEN                      (0xC00 + 0xC)

// TX APP -> BT
#define M2B_CH0_REGION_START              (IPC_REGION_START)
#define M2B_CH0_REGION_LEN                CTL_CHL_LEN
#define M2B_CH1_REGION_START              (M2B_CH0_REGION_START + M2B_CH0_REGION_LEN)
#define M2B_CH1_REGION_LEN                DATA_CHL_LEN
// RX BT -> APP
#define B2M_CH0_REGION_START              (M2B_CH1_REGION_START + M2B_CH1_REGION_LEN)
#define B2M_CH0_REGION_LEN                CTL_CHL_LEN
#define B2M_CH1_REGION_START              (B2M_CH0_REGION_START + B2M_CH0_REGION_LEN)
#define B2M_CH1_REGION_LEN                DATA_CHL_LEN
// TX APP -> DSP
#define M2D_CH0_REGION_START              (B2M_CH1_REGION_START + B2M_CH1_REGION_LEN)
#define M2D_CH0_REGION_LEN                CTL_CHL_LEN
#define M2D_CH1_REGION_START              (M2D_CH0_REGION_START + M2D_CH0_REGION_LEN)
#define M2D_CH1_REGION_LEN                DATA_CHL_LEN
// RX DSP -> APP
#define D2M_CH0_REGION_START              (M2D_CH1_REGION_START + M2D_CH1_REGION_LEN)
#define D2M_CH0_REGION_LEN                CTL_CHL_LEN
#define D2M_CH1_REGION_START              (D2M_CH0_REGION_START + D2M_CH0_REGION_LEN)
#define D2M_CH1_REGION_LEN                DATA_CHL_LEN

// BT <=======> DSP
#define B2D_CH0_REGION_START              (D2M_CH1_REGION_START + D2M_CH1_REGION_LEN)
#define B2D_CH0_REGION_LEN                CTL_CHL_LEN
#define B2D_CH1_REGION_START              (B2D_CH0_REGION_START + B2D_CH0_REGION_LEN)
#define B2D_CH1_REGION_LEN                DATA_CHL_LEN
#define D2B_CH0_REGION_START              (B2D_CH1_REGION_START + B2D_CH1_REGION_LEN)
#define D2B_CH0_REGION_LEN                CTL_CHL_LEN
#define D2B_CH1_REGION_START              (D2B_CH0_REGION_START + D2B_CH0_REGION_LEN)
#define D2B_CH1_REGION_LEN                DATA_CHL_LEN

#define IPC_REGION_START_LEN               ((6 * CTL_CHL_LEN) + (6 * DATA_CHL_LEN))  // 0x6090

#define EFUSE_SAVE_VAL                     (IPC_REGION_START + IPC_REGION_START_LEN)
#define EFUSE_SAVE_VAL_LEN                 0x120    // efuse: 284 byte -> 2272 bit

/* PLATFORM区域 */
#define PLT_REGION_START                   (EFUSE_SAVE_VAL + EFUSE_SAVE_VAL_LEN)
/* 256 bytes for all core preserve region */
#define PRESERVED_REGION_ORIGIN            PLT_REGION_START
#define PRESERVED_REGION_LENGTH            (0x100)
/* 256 bytes for App core preserve region */
#define APP_PRESERVED_REGION_ORIGIN        (PRESERVED_REGION_ORIGIN + PRESERVED_REGION_LENGTH)
#define APP_PRESERVED_REGION_LENGTH        (0x100)
/* System config region. */
#define SYSTEM_CFG_REGION_START            (APP_PRESERVED_REGION_ORIGIN + APP_PRESERVED_REGION_LENGTH)
#define SYSTEM_CFG_REGION_LENGTH           (0x40)
/* Reboot magic region. */
#define REBOOT_MAGIC_START                 (SYSTEM_CFG_REGION_START + SYSTEM_CFG_REGION_LENGTH)
#define REBOOT_MAGIC_LENGTH                (0x80)
#define PLT_REGION_LEN                     (PRESERVED_REGION_LENGTH + APP_PRESERVED_REGION_LENGTH + \
                                            SYSTEM_CFG_REGION_LENGTH + REBOOT_MAGIC_LENGTH)

/* Creash info 区域 */
#define CRASH_MEM_REGION_START             (PLT_REGION_START + PLT_REGION_LEN)
#define CH_APP_REGION_START                CRASH_MEM_REGION_START
#define CH_BT_REGION_START                 (CH_APP_REGION_START + CH_APP_REGION_LEN)

#define CRASH_MEM_REGION_LENGTH            (CH_BT_REGION_LEN + CH_APP_REGION_LEN)

/* LOG区域 BT(7K)、APP(3K)、DSP(8K) */
#define LOGGING_REGION_START                (CRASH_MEM_REGION_START + CRASH_MEM_REGION_LENGTH)
#define BT_LOGGING_REGION_START             LOGGING_REGION_START
#define APP_LOGGING_REGION_START            (BT_LOGGING_REGION_START + BT_LOGGING_LENGTH)
#define DSP_LOGGING_REGION_START            (APP_LOGGING_REGION_START + APP_LOGGING_LENGTH)
#define LOGGING_REGION_LENGTH               (BT_LOGGING_LENGTH + APP_LOGGING_LENGTH + DSP_LOGGING_LENGTH)

/* MessData区域 */
#define MASSDATA_REGION_START               (LOGGING_REGION_START + LOGGING_REGION_LENGTH)
#define MASSDATA_REGION_LENGTH              (BT_MASSDATA_LENGTH + APP_MASSDATA_LENGTH)

/* bt flag 64 byte */
#define BT_FLAG_REGION_START                (MASSDATA_REGION_START + MASSDATA_REGION_LENGTH)
#define BT_FLAG_REGION_LENGTH               (0x40)

/* Audio Base区域, 8584 bytes */
#define AUDIO_BASE_REGION_START                (BT_FLAG_REGION_START + BT_FLAG_REGION_LENGTH)
#define AUDIO_BASE_REGION_LENGTH               (0x2188)

/* Trng data shared memory 512 byte */
#define TRNG_DATA_REGION_START          ((AUDIO_BASE_REGION_START + AUDIO_BASE_REGION_LENGTH))
#define TRNG_DATA_REGION_LENGTH         (0x200)

/*  temperature info share memory 16 byte */

#define TEMPERATURE_DATA_REGION_START          ((TRNG_DATA_REGION_START + TRNG_DATA_REGION_LENGTH))
#define TEMPERATURE_DATA_REGION_LENGTH         (0x10)

/* RESERVED share mem */
#define RESERVED_SHARE_MEM_ORIGIN (TEMPERATURE_DATA_REGION_START + TEMPERATURE_DATA_REGION_LENGTH)
#define RESERVED_SHARE_MEM_LENGTH \
    (SHARED_MEM_START + SHARED_MEM_LENGTH - AUDIO_DATA_STREAM_REGION_LENGTH - RESERVED_SHARE_MEM_ORIGIN)

#if (RESERVED_SHARE_MEM_LENGTH < 0)
#error Share memory overflow
#endif

/* Audio data区域, 64KB */
#define AUDIO_DATA_STREAM_REGION_START      (SHARED_MEM_START + SHARED_MEM_LENGTH - AUDIO_DATA_STREAM_REGION_LENGTH)
#endif /* MEMORY_CONFIG_SHARED_H */

