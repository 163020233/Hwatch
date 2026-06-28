/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description:  Flash memory configurations
 * Author: @CompanyNameTag
 */

#ifndef MEMORY_CONFIG_FLASH_H
#define MEMORY_CONFIG_FLASH_H

/**
 *                             all image in norFlash(16M) layout
 *  index       region name                     start addr      length      size            page
 *  00          root key & partition cfg        0x26000000      0x1000      4K              1
 *  01          ssb image                       0x26001000      0x10000     64K             16
 *  02          ssb backup image                0x26011000      0x10000     64K             16
 *  04          recovery image                  0x2601F000      0x5F000     376K            95
 *  03          selitos image                   0x26080000      0x40000     256K            64
 *  05          application image               0x260C0000      0x800000    8M(max:12.42M)  2048(max:3179)
 *  06          fota                            0x26D2B000      0x54000     336K            84
 *  07          dfx                             0x26D7F000      0x3000      12K             3
 *  08          nv                              0x26D82000      0x6000      24K             6
 *  09          bt image                        0x26D88000      0x78000     480K            120
 *  10          dsp main image                  0x26E00000      0x80000     512K            128
 *  11          dsp overlay image               0x26E80000      0x180000    1.5M            384
 */

/**
 * @defgroup connectivity_config_memory MEMORY
 * @ingroup  connectivity_config
 * @{
 */
#define FLASH_START                     0x26000000
#define FLASH_1_START                   0x26000000
#define FLASH_LENGTH                    0x1000000

#define FLASH_MAX_END                   (FLASH_START + FLASH_LENGTH)

#define FLASH_PAGE_SIZE                 4096

#define FLASH_SIGN_KEY_AREA_LENGTH      0x100
#define FLASH_SIGN_CODE_INFO_LENGTH     0x200
#define FLASH_SIGN_HEADER               (FLASH_SIGN_KEY_AREA_LENGTH + FLASH_SIGN_CODE_INFO_LENGTH)

/* ----------------------------------------------------------------------------------------------------------------- */
/* IMAGE Area Defines */
/* MCU SSB Region */
#define KV_PAGES                        0

#define ROOTKRY_PARTITION_CCF_LENGTH    0x1000    /* flash 4KB align */
#define ROOT_PUBLIC_KEY_START           (FLASH_START)
#define ROOT_PUBLIC_KEY_LEN             0x80
/* partition */
#define PARTITION_TABLE_REGION_START    (ROOT_PUBLIC_KEY_START + ROOT_PUBLIC_KEY_LEN)
#define PARTITION_INFO_REGION_START     (PARTITION_TABLE_REGION_START + FLASH_SIGN_HEADER)
#define PARTITION_TABLE_REGION_LENGTH   (0x400 + FLASH_SIGN_HEADER)
#define CHIP_CONFIG_START               (PARTITION_TABLE_REGION_START + PARTITION_TABLE_REGION_LENGTH)
#define CHIP_CONFIG_LENGTH              0x308

#define SSB_FLASH_REGION_PAGES          (SSB_IMAGE_PAGES)
#define SSB_FLASH_REGION_START          (ROOT_PUBLIC_KEY_START + ROOTKRY_PARTITION_CCF_LENGTH)
#define SSB_FLASH_REGION_LENGTH         (SSB_FLASH_REGION_PAGES * FLASH_PAGE_SIZE)

#define align_up(addr, align)           (((addr) + (align) - 1) & (~((align) - 1)))

/* Flash load addr */
#define SSB_VECTORS_LOAD_ORIGIN         (SSB_FLASH_REGION_START + FLASH_SIGN_HEADER)
#define SSB_VECTORS_LENGTH              (0x300)
#define SSB_VERSION_LOAD_ORIGIN         (SSB_VECTORS_LOAD_ORIGIN + SSB_VECTORS_LENGTH)
#define SSB_VERSION_LENGTH              (0x60)
#define SSB_CHANGEID_LOAD_ORIGIN        (SSB_VERSION_LOAD_ORIGIN + SSB_VERSION_LENGTH)
#define SSB_CHANGEID_LENGTH             0x10

#define SSB_PROGRAM_ORIGIN              (align_up((SSB_CHANGEID_LOAD_ORIGIN + SSB_CHANGEID_LENGTH), 64))
#define SSB_PROGRAM_LENGTH              (SSB_FLASH_REGION_LENGTH - SSB_PROGRAM_ORIGIN)

/* MCU SSB BACK-UP Region */
#define SSB_BACKUP_FLASH_REGION_PAGES   (SSB_BACKUP_IMAGE_PAGES)
#define SSB_BACKUP_FLASH_REGION_START   (SSB_FLASH_REGION_START + SSB_FLASH_REGION_LENGTH)
#define SSB_BACKUP_FLASH_REGION_LENGTH  (SSB_BACKUP_FLASH_REGION_PAGES * FLASH_PAGE_SIZE)
#define SSB_BACKUP_VECTORS_LOAD_ORIGIN  (SSB_BACKUP_FLASH_REGION_START + FLASH_SIGN_HEADER)
#define SSB_BACKUP_VERSION_LOAD_ORIGIN  (SSB_BACKUP_VECTORS_LOAD_ORIGIN + SSB_VECTORS_LENGTH)
#define SSB_BACKUP_CHANGEID_LOAD_ORIGIN (SSB_BACKUP_VERSION_LOAD_ORIGIN + SSB_VERSION_LENGTH)
#define SSB_BACKUP_PROGRAM_ORIGIN       (align_up((SSB_BACKUP_CHANGEID_LOAD_ORIGIN + SSB_CHANGEID_LENGTH), 64))
#define SSB_BACKUP_PROGRAM_LENGTH       (SSB_BACKUP_FLASH_REGION_LENGTH - SSB_BACKUP_PROGRAM_ORIGIN)

/* RECOVERY Region */
#define RECOVERY_FLASH_REGION_PAGES     (RECOVERY_IMAGE_PAGES)
#define RECOVERY_FLASH_REGION_START     (SSB_BACKUP_FLASH_REGION_START + SSB_BACKUP_FLASH_REGION_LENGTH)
#define RECOVERY_FLASH_REGION_LENGTH    (RECOVERY_FLASH_REGION_PAGES * FLASH_PAGE_SIZE)

#define RECOVERY_VECTORS_LOAD_ORIGIN    (RECOVERY_FLASH_REGION_START  + FLASH_SIGN_HEADER)
#define RECOVERY_PROGRAM_ORIGIN         (RECOVERY_VECTORS_LOAD_ORIGIN + APP_VECTORS_LENGTH)
#define RECOVERY_PROGRAM_LENGTH         ((RECOVERY_FLASH_REGION_LENGTH) - \
                                        (APP_VECTORS_LENGTH + FLASH_SIGN_HEADER))

/* SELITEOS Region */
#define SELITEOS_FLASH_REGION_PAGES     (SELITEOS_IMAGE_PAGES)
#define SELITEOS_FLASH_REGION_START     (RECOVERY_FLASH_REGION_START + RECOVERY_FLASH_REGION_LENGTH)
#define SELITEOS_FLASH_REGION_LENGTH    (SELITEOS_FLASH_REGION_PAGES * FLASH_PAGE_SIZE)

#define SELITEOS_PROGRAM_ORIGIN         (SELITEOS_FLASH_REGION_START + FLASH_SIGN_HEADER)
#define SELITEOS_ITCM_LENGTH            0x18000
#define SELITEOS_PROGRAM_LENGTH         (SELITEOS_ITCM_LENGTH - FLASH_SIGN_HEADER)
#define SELITEOS_PROGRAM_EXT            (SELITEOS_PROGRAM_ORIGIN + SELITEOS_PROGRAM_LENGTH)
#define SELITEOS_PROGRAM_EXT_LENGTH     (SELITEOS_FLASH_REGION_LENGTH - SELITEOS_ITCM_LENGTH)

/* APP Region */
#define APP_FLASH_REGION_PAGES          (APP_IMAGE_PAGES)
#define APP_FLASH_REGION_START          (SELITEOS_FLASH_REGION_START + SELITEOS_FLASH_REGION_LENGTH)
#define APP_FLASH_REGION_LENGTH         (APP_FLASH_REGION_PAGES * FLASH_PAGE_SIZE)

#define APP_VECTORS_LOAD_ORIGIN         (APP_FLASH_REGION_START + FLASH_SIGN_HEADER)
#define APP_VECTORS_LENGTH              0x800
#define APP_PROGRAM_ORIGIN              (APP_VECTORS_LOAD_ORIGIN + APP_VECTORS_LENGTH)
#define APP_PROGRAM_LENGTH              ((APP_FLASH_REGION_LENGTH) - (APP_VECTORS_LENGTH + FLASH_SIGN_HEADER))
#ifndef USE_PSRAM_INSTEAD_NORFLASH_FOR_CODETEXT
#define REMOTE_FUNC_ADDR                APP_PROGRAM_ORIGIN
#endif

/* Reserved Region */
#define APP_RESERVED_FLASH_REGION_PAGES     (APP_RESERVED_IMAGE_PAGES)
#define APP_RESERVED_FLASH_REGION_START     (APP_FLASH_REGION_START + APP_FLASH_REGION_LENGTH)
#define APP_RESERVED_FLASH_REGION_LENGTH    (APP_RESERVED_FLASH_REGION_PAGES * FLASH_PAGE_SIZE)

/* ----------------------------------------------------------------------------------------------------------------- */
/* FOTA Region Defines */
#define FOTA_FLASH_REGION_PAGES         (FOTA_IMAGE_PAGES)
#define FOTA_FLASH_REGION_START         (APP_RESERVED_FLASH_REGION_START + APP_RESERVED_FLASH_REGION_LENGTH)
#define FOTA_FLASH_REGION_LENGTH        (FOTA_FLASH_REGION_PAGES * FLASH_PAGE_SIZE)

/* UPG Store Image Region */
#define UPG_FLASHBOOT_FLASH_REGION_PAGES            (UPG_FLASHBOOT_IMAGE_PAGES)
#define UPG_FLASHBOOT_FLASH_REGION_START            (FOTA_FLASH_REGION_START + FOTA_FLASH_REGION_LENGTH)
#define UPG_FLASHBOOT_FLASH_REGION_LENGTH           (UPG_FLASHBOOT_IMAGE_PAGES * FLASH_PAGE_SIZE)

#define UPG_SELITEOS_FLASH_REGION_PAGES             (UPG_SELITEOS_IMAGE_PAGES)
#define UPG_SELITEOS_FLASH_REGION_START             (UPG_FLASHBOOT_FLASH_REGION_START + \
                                                        UPG_FLASHBOOT_FLASH_REGION_LENGTH)
#define UPG_SELITEOS_FLASH_REGION_LENGTH            (UPG_SELITEOS_FLASH_REGION_PAGES * FLASH_PAGE_SIZE)

/* DFX Region Defines */
#define DFX_FLASH_REGION_PAGES          (DFX_STORE_PAGES)
#define DFX_FLASH_REGION_START          (UPG_SELITEOS_FLASH_REGION_START + UPG_SELITEOS_FLASH_REGION_LENGTH)
#define DFX_FLASH_REGION_LENGTH         (DFX_STORE_PAGES * FLASH_PAGE_SIZE)

/* KV Region Defines */
#define NV_STATR_ADDR                   (DFX_FLASH_REGION_START + DFX_FLASH_REGION_LENGTH)
#define NV_LENGTH                       (NV_PAGES * FLASH_PAGE_SIZE)

/* KV Backup Region Defines */
#define NV_BACKUP_STATR_ADDR            (NV_STATR_ADDR + NV_LENGTH)
#define NV_BACKUP_LENGTH                (NV_BACKUP_PAGES * FLASH_PAGE_SIZE)

/* cores image flash region start */
#define FLASH_CORE_IMAGES_START         (FLASH_START + SSB_FLASH_REGION_LENGTH + SSB_BACKUP_FLASH_REGION_LENGTH + \
                                        ROOTKRY_PARTITION_CCF_LENGTH)
/* ----------------------------------------------------------------------------------------------------------------- */
/* BT Region */
#define BT_FLASH_REGION_PAGES           (BT_IMAGE_PAGES) // 0x78000
#define BT_FLASH_REGION_START           (NV_BACKUP_STATR_ADDR + NV_BACKUP_LENGTH)
#define BT_FLASH_REGION_LENGTH          (BT_IMAGE_PAGES * FLASH_PAGE_SIZE) // 0x78000

#ifdef MASTER_LOAD_SLAVE
#define BUILD_INFO_LENGRH               408
#else
#define BUILD_INFO_LENGRH               0
#endif
#define BT_BUILD_INFO_ORIGIN            (BT_FLASH_REGION_START + FLASH_SIGN_HEADER)
#define BT_VECTORS_LOAD_ORIGIN          (BT_BUILD_INFO_ORIGIN + BUILD_INFO_LENGRH)
#define BT_VECTORS_LENGTH               0x200
#define FEATURE_TABLE_LOAD_ORIGIN       (BT_VECTORS_LOAD_ORIGIN + BT_VECTORS_LENGTH)
#define FEATURE_TABLE_LENGTH            0x200
#define BT_VERSION_LOAD_ORIGIN          (FEATURE_TABLE_LOAD_ORIGIN + FEATURE_TABLE_LENGTH)
#define BT_VERSION_LENGTH               88
#define BT_PROGRAM_ORIGIN               (BT_VERSION_LOAD_ORIGIN + BT_VERSION_LENGTH)
#define BT_PROGRAM_LENGTH               ((BT_FLASH_REGION_LENGTH) - \
                                        (BT_VECTORS_LENGTH + FEATURE_TABLE_LENGTH + BUILD_INFO_LENGRH + FLASH_SIGN_HEADER))

/* DSP0 Region */
#define DSP0_FLASH_REGION_PAGES         (HIFI0_IMAGE_PAGES)
#define DSP0_FLASH_REGION_START         (BT_FLASH_REGION_START + BT_FLASH_REGION_LENGTH)
#define DSP0_FLASH_REGION_LENGTH        (DSP0_FLASH_REGION_PAGES * FLASH_PAGE_SIZE)

#define DSP0_LOAD_SECTION_OFFSET        0x7c
#define DSP0_LOAD_SECTION_START         (DSP0_FLASH_REGION_START + DSP0_LOAD_SECTION_OFFSET)

/* DSP1 Region */
#define DSP_CODEC_FLASH_REGION_PAGES    (HIFI0_CODEC_PAGES)
#define DSP_CODEC_FLASH_REGION_START    (DSP0_FLASH_REGION_START + DSP0_FLASH_REGION_LENGTH)
#define DSP_CODEC_FLASH_REGION_LENGTH   (DSP_CODEC_FLASH_REGION_PAGES * FLASH_PAGE_SIZE)
/* ----------------------------------------------------------------------------------------------------------------- */
/**
 * @}
 */
#endif
