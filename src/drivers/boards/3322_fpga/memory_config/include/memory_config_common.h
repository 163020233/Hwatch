/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:  Default memory configurations
 * Author: @CompanyNameTag
 * Create:  2022-03-09
 */

#ifndef MEMORY_CONFIG_COMMON_H
#define MEMORY_CONFIG_COMMON_H

#include "product.h"
#include "chip_definitions.h"
#include "memory_config_flash.h"
#include "memory_config_shared.h"
#include "psram.h"

/**
 * @defgroup connectivity_config_memory MEMORY
 * @ingroup  connectivity_config
 * @{
 */

/*
 * ********************* APP ***********************
 */
/* 48K ROM */
#define ROM_START                       0x00000000
#define ROM_LENGTH                      0xc000

/* 256K APP ITCM config */
#define APP_ITCM_ORIGIN                 0x10000
#define APP_ITCM_LENGTH                 0x40000

/* 128K TEE ITCM config */
#define TEE_ITCM_ORIGIN                 0x50000
#define TEE_ITCM_LENGTH                 0x20000

/* 640K APP DTCM config */
#define APP_DTCM_ORIGIN                 0x10000000
#define APP_DTCM_LENGTH                 0xA0000

/* TEE DTCM config */
#define TEE_DTCM_ORIGIN                 0x10000000
#define TEE_DTCM_LENGTH                 0x10000
#define UPG_TEE_DTCM_LENGTH             0x8000

#define REE_ITCM_ORIGIN                 APP_ITCM_ORIGIN
#define REE_ITCM_LENGTH                 APP_ITCM_LENGTH
#define REE_DTCM_ORIGIN                 (APP_DTCM_ORIGIN + TEE_DTCM_LENGTH)
#define REE_DTCM_LENGTH                 (APP_DTCM_LENGTH - TEE_DTCM_LENGTH)

/* APP ITCM config */
#define MCU_ITCM_END                    (APP_ITCM_ORIGIN + APP_ITCM_LENGTH)

/* the minimum APP heap size */
#define APP_HEAP_MINIMUM_SIZE           (APP_HEAP_SIZE)

/*
 * ********************* BT ***********************
 */
#define BT_RAM_START                    0x0
#ifdef DEVICE_ONLY
#define BT_RAM_LENGTH                   0x88000
#else
#define BT_RAM_LENGTH                   0x88000
#endif
#define BT_RAM_END                      (BT_RAM_START + BT_RAM_LENGTH)

/* bt rom config */
#define BT_ROM_START                    0xA0000
#define BT_ROM_LENGTH                   0x60000

/* APP Core can also access the BT core RAM, need mapping out. */
#define BT_RAM_ORIGIN_APP_MAPPING           0x82000000
#define BT_RAM_ORIGIN_APP_MAPPING_LENGTH    BT_RAM_LENGTH

#define BT_ROM_ORIGIN_APP_MAPPING           0x820A0000
#define BT_ROM_ORIGIN_APP_MAPPING_LENGTH    BT_ROM_LENGTH

/* Define the Vectors table location in ram, used by link script. */
/* BCPU VECTORS region */
#define BT_VECTORS_ORIGIN               (BT_RAM_START)

#define BT_PROGRAM_RAM_ORIGIN           (BT_RAM_START + BT_VECTORS_LENGTH)
#define BT_PROGRAM_RAM_LENGTH           (BT_RAM_LENGTH - BT_VECTORS_LENGTH)

#define BT_USER_STACK_LEN               0
#define BT_IRQ_STACK_LEN                0xC00
#define BT_EXCP_STACK_LEN               0xC00
#define BT_NMI_STACK_LEN                0XC00
#define BT_STACK_LEN                    (BT_USER_STACK_LEN + BT_IRQ_STACK_LEN + BT_EXCP_STACK_LEN + BT_NMI_STACK_LEN)

/* the minimum BT heap size (50kB) */
#define BT_HEAP_MINIMUM_SIZE            0xC800

/* BT_L2M_MEM */
#define BT_L2M_MEM_ORIGIN 0x02030000
#define BT_L2M_MEM_LENGTH 0x30000
#define BT_L2M_MEM_ORIGIN_APP_MAPPING (0x20000000 - 0x02000000)
#define BT_L2M_MEM_ORIGIN_APP_MAPPING_LENGTH BT_L2M_MEM_LENGTH

/*
 * ********************* ATE ***********************
 */
/* APP PROGRAM LOAD */
#define APP_ATE_PROGRAM_ORIGIN          (APP_ITCM_ORIGIN)
#define APP_ATE_PROGRAM_LENGTH          (0x10000)

/* ATE cmd param 256 bytes */
#define APP_ATE_PARAM_ORIGIN            (APP_DTCM_ORIGIN)
#define APP_ATE_PARAM_LENGTH            (0x100)

/* ATE cmd result 256 bytes for max support 64 commands */
#define APP_ATE_RESULT_ORIGIN           (APP_ATE_PARAM_ORIGIN + APP_ATE_PARAM_LENGTH)
#define APP_ATE_RESULT_LENGTH           (0x100)

/* ATE cmd data 256 bytes for max support 64 word */
#define APP_ATE_DATA_REG_ORIGIN         (APP_ATE_RESULT_ORIGIN + APP_ATE_RESULT_LENGTH)
#define APP_ATE_DATA_REG_LENGTH         (0x100)

/* ATE psram default param 8 bytes */
#define APP_ATE_PSRAM_PARAM_ORIGIN      (APP_ATE_DATA_REG_ORIGIN + APP_ATE_DATA_REG_LENGTH)
#define APP_ATE_PSRAM_PARAM_LENGTH      (0x08)

/* ATE psram eid param 8 bytes */
#define APP_ATE_PSRAM_EID_ORIGIN        (APP_ATE_PSRAM_PARAM_ORIGIN + APP_ATE_PSRAM_PARAM_LENGTH)
#define APP_ATE_PSRAM_EID_LENGTH        (0x08)

#define APP_ATE_USE_DTCM_ORIGIN         (APP_ATE_PSRAM_EID_ORIGIN + APP_ATE_PSRAM_EID_LENGTH)
#define APP_ATE_USE_DTCM_LENGTH         (0x10000)

#define APP_ATE_USE_ITCM_ORIGIN         (APP_ATE_PROGRAM_ORIGIN + APP_ATE_PROGRAM_LENGTH)
#define APP_ATE_USE_ITCM_LENGTH         (0x10000)

/* BT ITCM config */
#define BT_ITCM_ORIGIN                  0x0
#define BT_ITCM_LENGTH                  0x10000
#define BT_ITCM_END                     (BT_ITCM_ORIGIN + BT_ITCM_LENGTH)

/* BT PROGRAM LOAD */
#define BT_ATE_VECTOR_ORIGIN            (BT_ITCM_ORIGIN)
#define BT_ATE_VECTOR_LENGTH            (0x100)

/* ATE cmd param 256 bytes */
#define BT_ATE_PARAM_ORIGIN             (BT_ATE_VECTOR_ORIGIN + BT_ATE_VECTOR_LENGTH)
#define BT_ATE_PARAM_LENGTH             (0x100)

/* ATE cmd result 256 bytes for max support 64 commands */
#define BT_ATE_RESULT_ORIGIN            (BT_ATE_PARAM_ORIGIN + BT_ATE_PARAM_LENGTH)
#define BT_ATE_RESULT_LENGTH            (0x100)

/* ATE cmd data 256 bytes for max support 64 word */
#define BT_ATE_DATA_REG_ORIGIN          (BT_ATE_RESULT_ORIGIN + BT_ATE_RESULT_LENGTH)
#define BT_ATE_DATA_REG_LENGTH          (0x100)

#define BT_ATE_PROGRAM_ORIGIN           (BT_ATE_DATA_REG_ORIGIN + BT_ATE_DATA_REG_LENGTH)
#define BT_ATE_PROGRAM_LENGTH           (0x10000)

#define BT_ATE_USE_RAM_ORIGIN           (BT_ATE_PROGRAM_ORIGIN + BT_ATE_PROGRAM_LENGTH)
#define BT_ATE_USE_RAM_LENGTH           (0x10000)

/* stack total len 10k */
#define ATE_STACK_TOTAL_LEN             (ATE_USER_STACK_LEN + ATE_IRQ_STACK_LEN + ATE_EXCP_STACK_LEN)
/* stack for normal 5k */
#define ATE_USER_STACK_LEN              0x1400
/* stack for irq 4k */
#define ATE_IRQ_STACK_LEN               0x1000
/* stack for exception 1k */
#define ATE_EXCP_STACK_LEN              0x400
/*
 * ********************* APP ***********************
 */
#define APP_RAMTEXT_ORIGIN              (APP_VECTORS_ORIGIN + APP_VECTORS_LENGTH)
#define APP_RAMTEXT_END                 (APP_ITCM_ORIGIN + APP_ITCM_LENGTH)
#define APP_RAMTEXT_LENGTH              (APP_RAMTEXT_END - APP_RAMTEXT_ORIGIN)

/* stack for normal 3k */
#define APP_USER_STACK_BASEADDR         APP_DTCM_ORIGIN
#define APP_USER_STACK_LEN              0xC00
#define APP_USER_STACK_LIMIT            (APP_USER_STACK_BASEADDR + APP_USER_STACK_LEN)

/* stack for irq 4k */
#define APP_IRQ_STACK_BASEADDR          APP_USER_STACK_LIMIT
#define APP_IRQ_STACK_LEN               0xC00
#define APP_IRQ_STACK_LIMIT             (APP_IRQ_STACK_BASEADDR + APP_IRQ_STACK_LEN)

/* stack for exception 4k */
#define APP_EXCP_STACK_BASEADDR         APP_IRQ_STACK_LIMIT
#define APP_EXCP_STACK_LEN              0xC00
#define APP_EXCP_STACK_LIMIT            (APP_EXCP_STACK_BASEADDR + APP_EXCP_STACK_LEN)

/* stack for nmi 4k */
#define APP_NMI_STACK_BASEADDR          APP_EXCP_STACK_LIMIT
#define APP_NMI_STACK_LEN               0xC00
#define APP_NMI_STACK_LIMIT             (APP_NMI_STACK_BASEADDR + APP_NMI_STACK_LEN)

/* 448K DTCM for APP core data, start at end of vectors table */
#define APP_RAM_ORIGIN                  (APP_NMI_STACK_LIMIT)
#define APP_RAM_END                     (APP_DTCM_ORIGIN + APP_DTCM_LENGTH)
#define APP_RAM_LENGTH                  (APP_RAM_END - APP_RAM_ORIGIN)

/* Define the Vectors table location in ram, used by link script. */
/* APP CPU VECTORS region */
#define APP_VECTORS_ORIGIN              (APP_ITCM_ORIGIN)

/* ----------------------------------------------------------------------------------------------------------------- */
/* DSP RAM use and manage by DSP team, M7 just need to know the access mapping. */
#define DSP_CODE_ADDR_BT_MAPPING        0x58000000

#define DSP_OCRAM_ACORE_PHYS_ADDR       0x58100000
#define DSP_OCRAM_PHYS_SIZE             0x60000
#define DSP_ITCM_MAP_ADDR               0x5803C000
#define DSP_ITCM_SIZE                   0xC000
#define DSP_DTCM_MAP_ADDR               0x58014000
#define DSP_DTCM_SIZE                   0x28000

/* 12*N bytes for cpu trace, trace line is 12 bytes, 8184 bytes */
#define MCPU_TRACE_MEM_REGION_START     0x52005000
#define BCPU_TRACE_MEM_REGION_START     0x5900D000

#if CORE == BT
#define CPU_TRACE_MEM_REGION_LENGTH 0x2100
#elif CORE == APPS
#define CPU_TRACE_MEM_REGION_LENGTH 0x1200
#endif

/*
 * ********************* ADDITIONAL MEMORY CONFIGURATION DEFINITIONS *********************
 * The offset in the BCPU (either flash or ROM) to where the version information starts.
 */
#define VERSION_INFORMATION_OFFSET (BT_VECTORS_LENGTH)

/****************************************************************************************/

/**
 * @}
 */
#endif
