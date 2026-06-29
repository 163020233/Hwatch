/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: dsp reg
 * Author: audio
 */

#ifndef __MEM_ADDR_H__
#define __MEM_ADDR_H__

#include "chip_type.h"
#include "common_dsp.h"
#include "psram.h"

#ifdef BRANDY_PROT /* bt core core id */
#define SHRB_CORE_K AUDIO_CORE_MCU0
#define SHRB_CORE_U AUDIO_CORE_MCU0

#else /* acore core id */
#define SHRB_CORE_K AUDIO_CORE_CPU_K
#define SHRB_CORE_U AUDIO_CORE_CPU_K
#endif /* BRANDY_PROT */

/* common memory tlb in reset-vector.S */
#define COM_MEM_AUDIO_VIRT_ADDR 0xA3000000
#define COM_MEM_AUDIO_PHYS_ADDR 0x83000000
#define COM_MEM_ACORE_PHYS_ADDR 0x83000000
#define COM_MEM_MAP_SIZE 0x100000

#define SHM_ADDR COM_MEM_AUDIO_VIRT_ADDR
#if (SAP_OS_TYPE == os_type_xtensa)
#define SHM_MAP_ADDR COM_MEM_AUDIO_VIRT_ADDR
#else
#define SHM_MAP_ADDR COM_MEM_ACORE_PHYS_ADDR
#endif

#define SOC_L2M_AUDIO_VIRT_ADDR 0xA0000000
#define SOC_L2M_AUDIO_PHYS_ADDR 0x20000000
#define SOC_L2M_ACORE_PHYS_ADDR 0x20000000
#define SOC_L2M_PHYS_SIZE SIZE_384K

#define SOC_SHM_AUDIO_VIRT_ADDR 0x63000000
#define SOC_SHM_AUDIO_PHYS_ADDR 0x83000000
#define SOC_SHM_ACORE_PHYS_ADDR 0x83000000
#define SOC_SHM_PHYS_SIZE SIZE_128K

#define SOC_PSRAM_AUDIO_VIRT_ADDR 0xC8500000
#define SOC_PSRAM_AUDIO_PHYS_ADDR 0xE8500000
#define SOC_PSRAM_ACORE_PHYS_ADDR PSRAM_DSP_CODE_ADDR
#define SOC_PSRAM_AUDIO_SIZE      PSRAM_DSP_CODE_LENGTH
#define SOC_PSRAM_PHYS_SIZE       PSRAM_LENTH

#define SOC_OCRAM_AUDIO_VIRT_ADDR 0xC8100000
#define SOC_OCRAM_AUDIO_PHYS_ADDR 0xE8100000
#define SOC_OCRAM_ACORE_PHYS_ADDR 0x58100000
#define SOC_OCRAM_PHYS_SIZE SIZE_384K

#define SRAM_TEST_PHYS_SIZE SIZE_128K
#define SRAM_TEST_AUDIO_VIRT_ADDR (SOC_OCRAM_AUDIO_VIRT_ADDR + SOC_OCRAM_PHYS_SIZE - SRAM_TEST_PHYS_SIZE)
#define SRAM_TEST_AUDIO_PHYS_ADDR (SOC_OCRAM_AUDIO_PHYS_ADDR + SOC_OCRAM_PHYS_SIZE - SRAM_TEST_PHYS_SIZE)

/* tlb in reset-vector.S */
#define SOC_MEM_AUDIO_VIRT_ADDR 0xC8000000
#define SOC_MEM_AUDIO_PHYS_ADDR 0xE8000000
#define SOC_MEM_ACORE_PHYS_ADDR 0x58000000
#define SOC_MEM_MAP_SIZE        0x18000000

#define SRAM_AUDIO_VIRT_ADDR (SOC_OCRAM_AUDIO_PHYS_ADDR - SOC_MEM_AUDIO_PHYS_ADDR + SOC_MEM_AUDIO_VIRT_ADDR)
#define SRAM_AUDIO_PHYS_ADDR SOC_OCRAM_AUDIO_PHYS_ADDR
#define SRAM_ACORE_PHYS_ADDR SOC_OCRAM_ACORE_PHYS_ADDR

/* DSP DTCM map */
#define SOC_MEM_AUDIO_DTCM_REMAP_PHYS_ADDR  0xE8058000
#define SOC_MEM_AUDIO_DTCM_LOGIC_PHYS_ADDR  0xE8014000
#define SOC_MEM_AUDIO_DTCM_SIZE             0x28000

#define SRAM_ADDR        SRAM_AUDIO_VIRT_ADDR
#ifdef SAP_SEA_SEE_PHS_SUPPORT
#define SRAM_SIZE (SIZE_1M + SIZE_256K)
#else
#define SRAM_SIZE SIZE_1M
#endif

#define PSRAM_ADDR      SOC_PSRAM_AUDIO_VIRT_ADDR
#define PSRAM_MAP_SIZE  SOC_PSRAM_AUDIO_SIZE
#if (SAP_OS_TYPE == os_type_xtensa)
#define SRAM_MAP_ADDR   SRAM_AUDIO_VIRT_ADDR
#define PSRAM_MAP_ADDR  SOC_PSRAM_AUDIO_VIRT_ADDR
#else
#define SRAM_MAP_ADDR   SRAM_ACORE_PHYS_ADDR
#define PSRAM_MAP_ADDR  SOC_PSRAM_ACORE_PHYS_ADDR
#endif

#define DRAM_ADDR      0xe8058000
#define DRAM_SIZE      (SIZE_32K + SIZE_128K)
#if (SAP_OS_TYPE == os_type_xtensa)
#define DRAM_MAP_ADDR  DRAM_ADDR
#else
#define DRAM_MAP_ADDR  0x58014000
#endif
#define ACORE_DSP0_DRAM_ADDR DRAM_MAP_ADDR
#define ACORE_DSP1_DRAM_ADDR 0x00000000

#define IRAM_ADDR      0xe8080000
#define IRAM_SIZE      (SIZE_32K + SIZE_16K)
#if (SAP_OS_TYPE == os_type_xtensa)
#define IRAM_MAP_ADDR  IRAM_ADDR
#else
#define IRAM_MAP_ADDR  0x5803C000
#endif
#define ACORE_DSP0_IRAM_ADDR IRAM_MAP_ADDR
#define ACORE_DSP1_IRAM_ADDR 0x00000000

#ifdef SAP_FPGA
#include "product_fpga_standard.h"
#else /* evb */
#include "product_evb_standard.h"
#endif /* SAP_FPGA */
#include "memory_config_shared.h"

/* share memory on SOC Share MEM */
#define SOC_SYS_SHM_BASE_ADDR    AUDIO_BASE_REGION_START /* AUDIO_BASE_REGION在DSP下电时不会下电 */
#define SOC_SYS_SHM_BASE_SIZE    SAP_SHM_BASE_SIZE /* region for sys cmd(pts/pss/tws_config/probe/heart_beat...) */

/* drivers/boards/3322_evb/product/product_evb_standard.h */
/* drivers/boards/3322_evb/memory_config/include/memory_config_shared.h */
#define SHM0_ADDR      (AUDIO_DATA_STREAM_REGION_START)  /* 0x8300B670 : memory_config_shared.h */
#define SHM0_SIZE      (AUDIO_DATA_STREAM_REGION_LENGTH) /* 0x00010000 : product_evb_standard.h */
#define SOC_SYS_SHM_MAX_SIZE     SHM0_SIZE /* Region for audio data */

#define SOC_SYS_SHM_REG_BASE     (SOC_SYS_SHM_BASE_ADDR)
#define SOC_IPC_SYS_STATE_ADDR   (SOC_SYS_SHM_REG_BASE + SAP_SHM_SYS_CMD_OFFSET)
#define SOC_IPC_PROBE_STATUS_ADDR  (SOC_SYS_SHM_REG_BASE + SAP_SHM_PROBE_STATUS_OFFSET)
#define SOC_IPC_ADP_SHARE_BASE_ADDR (SOC_SYS_SHM_REG_BASE + SAP_SHM_ADP_IDS_OFFSET)
#define SOC_IPC_SHM_BASE_ADDR    (SOC_SYS_SHM_REG_BASE + SAP_SHM_SHRB_CFG_OFFSET)
#define SOC_SND_TRACK_PTS_ADDR   (SOC_SYS_SHM_REG_BASE + SAP_SHM_TRACK_PTS_OFFSET)
#define SOC_TWS_CONFIG_ADDR      (SOC_SYS_SHM_REG_BASE + SAP_SHM_TWS_CONFIG_OFFSET)
#define SOC_ADEC_PSS_ADDR        (SOC_SYS_SHM_REG_BASE + SAP_SHM_ADEC_PSS_OFFSET)

#define MAIN_SRAM_SIZE 0x00060000 /* 384KB */
#define DFX_SHM_SIZE 0x00000000   /* 0KB */
#define AUDIO_DRV_TCM_OVERLAY_BIN_SIZE 0x00000000 /* 0KB */

#define MAIN_SRAM_ADDR SRAM_MAP_ADDR
#define AUDIO_DRV_TCM_OVERLAY_ENTRY_ADDR PSRAM_MAP_ADDR

/* DSP firmware flash addr */
#define FLASH_BASEADDR           0x26000000
#define FLASH_ROOT_PUB_KEY_START (FLASH_BASEADDR)
#define FLASH_DSP_MAIN_OFFSET    0x8cc000
#define FLASH_DSP_OVERLAY_OFFSET 0x9cc000

#define FLASH_DSP_MAIN_ADDR      (FLASH_BASEADDR + FLASH_DSP_MAIN_OFFSET)
#define FLASH_DSP_MAIN_SIZE      (SIZE_1M) /* 0x00100000 */

#define FLASH_DSP_OVERLAY_ADDR   (FLASH_BASEADDR + FLASH_DSP_OVERLAY_OFFSET)
#define FLASH_DSP_OVERLAY_SIZE   (SIZE_2M + SIZE_512K) /* 0x00280000 */

#define FLASH_DSP_LIB_OFFSET   0x200000
#define FLASH_DSP_LIB_ADDR     FLASH_DSP_OVERLAY_ADDR
#define FLASH_DSP_LIB_SIZE     FLASH_DSP_OVERLAY_SIZE

#define MMZ_MEM_START_ADDR     0 /* mmzmem section in memmap.xmm */
#define MMZ_MEM_SIZE           0 /* zero byte */

/* mmz_blk0 : Services */
#define MMZ_BLOCK0_START_ADDR  (AUDIO_BASE_REGION_START + SOC_SYS_SHM_BASE_SIZE) /* msg_fifo region addr */
#define MMZ_BLOCK0_SIZE        (AUDIO_BASE_REGION_LENGTH - SOC_SYS_SHM_BASE_SIZE) /* msg_fifo region size */
/* mmz_blk1 : mmz mem */
#define MMZ_BLOCK1_START_ADDR  MMZ_MEM_START_ADDR
#define MMZ_BLOCK1_SIZE        MMZ_MEM_SIZE
/* mmz_blk2 : audio stream */
#define MMZ_BLOCK2_START_ADDR  SHM0_ADDR
#define MMZ_BLOCK2_SIZE        SHM0_SIZE
/* mmz_blk3-4 : reserved */
#define MMZ_BLOCK3_START_ADDR  0
#define MMZ_BLOCK3_SIZE        0
#define MMZ_BLOCK4_START_ADDR  0
#define MMZ_BLOCK4_SIZE        0
/* LOG Region */
/* BT - 7K, APP - 3K, DSP - 8K */
#define SAP_BT_LOGGING_LENGTH     BT_LOGGING_LENGTH      /* 0x00001C00 : product_evb_standard.h */
#define SAP_APP_LOGGING_LENGTH    APP_LOGGING_LENGTH     /* 0x00000C00 : product_evb_standard.h */
#define SAP_DSP_LOGGING_LENGTH    DSP_LOGGING_LENGTH     /* 0x00002000 : product_evb_standard.h */
#define SAP_LOGGING_REGION_START  LOGGING_REGION_START   /* 0x83006E70 : memory_config_shared.h */
#define SAP_LOGGING_REGION_LENGTH LOGGING_REGION_LENGTH  /* memory_config_shared.h */
#endif /* __MEM_ADDR_H__ */
