/*-
 * Copyright (c) 2014 Ruslan Bukin <br@bsdpad.com>
 * All rights reserved.
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract (FA8750-10-C-0237)
 * ("CTSRD"), as part of the DARPA CRASH research programme.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#ifndef MMC_PRI_H
#define MMC_PRI_H

#include "emmc_type.h"
#include "sdhci.h"

#define CMD_RETRIES 3

#define DEBUG
#ifdef DEBUG
#define bootverbose 1
#else
#define bootverbose 0
#endif

#define mmc_setbit(a, i) *(uint32_t *)(a) |= 1U << (i)
#define mmc_clrbit(a, i) *(uint32_t *)(a) &= ~(1U << (i))
#define mmc_isset(a, i) ((*(uint32_t *)(a) & (1U << (i))) != 0)
#define mmc_isclr(a, i) ((*(uint32_t *)(a) & (1U << (i))) == 0)

#define MMC_CARD_UNINIT 0
#define MMC_CARD_INIT 1
#define MMC_CARD_INIT_FAIL 2

#define EXT_CSD_PART_CONFIG_ACC_DEFAULT 0x00
#define EXT_CSD_PART_CONFIG_ACC_BOOT0 0x01
#define EXT_CSD_PART_CONFIG_ACC_BOOT1 0x02
#define EXT_CSD_PART_CONFIG_ACC_RPMB 0x03
#define EXT_CSD_PART_CONFIG_ACC_GP0 0x04
#define EXT_CSD_PART_CONFIG_ACC_GP1 0x05
#define EXT_CSD_PART_CONFIG_ACC_GP2 0x06
#define EXT_CSD_PART_CONFIG_ACC_GP3 0x07
#define EXT_CSD_PART_CONFIG_ACC_MASK 0x07
#define EXT_CSD_PART_CONFIG_BOOT0 0x08
#define EXT_CSD_PART_CONFIG_BOOT1 0x10
#define EXT_CSD_PART_CONFIG_BOOT_USR 0x38
#define EXT_CSD_PART_CONFIG_BOOT_MASK 0x38
#define EXT_CSD_PART_CONFIG_BOOT_ACK 0x40

#define SD_SWITCH_FUNC 6

/*
 * Card Command Classes
 */
#define CCC_BASIC (1U << 0)
#define CCC_BLOCK_READ (1U << 2)
#define CCC_BLOCK_WRITE (1U << 4)
#define CCC_ERASE (1U << 5)

#define SD_SCR_SPEC_V0 0
#define SD_SCR_SPEC_V1 1
#define SD_SCR_SPEC_V2 2

#define SD_SCR_BUS_WIDTH_1 (1 << 0)
#define SD_SCR_BUS_WIDTH_4 (1 << 2)

#define SD_SCR_CMD20_SUPPORT (1U << 0)
#define SD_SCR_CMD23_SUPPORT (1U << 1)

/*
 * SD bus widths
 */
#define SD_BUS_WIDTH_1 0
#define SD_BUS_WIDTH_4 2

/*
 * SD Switch
 */
#define SD_SWITCH_MODE_CHECK 0
#define SD_SWITCH_MODE_SET 1
#define SD_SWITCH_GROUP1 0 /* Access mode */
#define SD_SWITCH_NORMAL_MODE 0
#define SD_SWITCH_SDR12_MODE 0 /* 1.8V */
#define SD_SWITCH_HS_MODE 1
#define SD_SWITCH_SDR25_MODE 1
#define SD_SWITCH_SDR50_MODE 2
#define SD_SWITCH_SDR104_MODE 3
#define SD_SWITCH_DDR50_MODE 4
#define SD_SWITCH_NOCHANGE 0xF

#define SD_SWITCH_GROUP2 1 /* Command system */
#define SD_SWITCH_GROUP3 2 /* Driver strength */
#define SD_SWITCH_GROUP4 3 /* Power limit */
#define SD_SWITCH_MAX_CURRENT_200 0
#define SD_SWITCH_MAX_CURRENT_400 1
#define SD_SWITCH_MAX_CURRENT_600 2
#define SD_SWITCH_MAX_CURRENT_800 3

#define SD_SWITCH_GROUP5 4 /* reserved */
#define SD_SWITCH_GROUP6 5 /* reserved */

#define SD_CLR_CARD_DETECT 0
#define SD_SET_CARD_DETECT 1

#define SD_HS_MAX 50000000
#define SD_DDR50_MAX 50000000
#define SD_SDR12_MAX 25000000
#define SD_SDR25_MAX 50000000
#define SD_SDR50_MAX 100000000
#define SD_SDR104_MAX 208000000

#define MMC_MAX_CURR_LTD_200 0
#define MMC_MAX_CURR_LTD_400 1
#define MMC_MAX_CURR_LTD_600 2
#define MMC_MAX_CURR_LTD_800 3

#define EXT_CSD_CARD_TYPE_HS_26 0x0001
#define EXT_CSD_CARD_TYPE_HS_52 0x0002
#define EXT_CSD_CARD_TYPE_DDR_52_1_8V 0x0004
#define EXT_CSD_CARD_TYPE_DDR_52_1_2V 0x0008
#define EXT_CSD_CARD_TYPE_HS200_1_8V 0x0010
#define EXT_CSD_CARD_TYPE_HS200_1_2V 0x0020
#define EXT_CSD_CARD_TYPE_HS400_1_8V 0x0040
#define EXT_CSD_CARD_TYPE_HS400_1_2V 0x0080

#define MMC_TYPE_HS_26_MAX 26000000
#define MMC_TYPE_HS_52_MAX 52000000
#define MMC_TYPE_DDR52_MAX 52000000
#define MMC_TYPE_HS200_HS400ES_MAX 200000000

#define EXT_CSD_STROBE_SUPPORT_EN 0x01

#define EXT_CSD_ERASE_GRP_DEF_EN 0x01

#define MMC_SDIO_BLOCK_SIZE 512
/* Maximum blocks IO_RW_EXTENDED is 511 */
#define MAX_SDIO_BLOCK_NUM 511
/* Maximum FUNS for sdio is 7 */
#define MAX_SDIO_FUN_NUM 7
#define FUNC_ADDR_BASE(f) ((f) << 8)

#define MMC_QUIRK_MID_ANY ((uint32_t)-1)
#define MMC_QUIRK_OID_ANY ((uint16_t)-1)
#define MMC_QUIRK_INAND_CMD38 0x0001
#define MMC_QUIRK_BROKEN_TRIM 0x0002
#define MMC_QUIRK_BROKEN_CMD23 4U
#define MMC_QUIRK_NON_STD_FUNC 8U
#define MMC_QUIRK_DIS_CD 0x10U

#define EXT_CSD_POWER_CLASS_8BIT_MASK 0xf0
#define EXT_CSD_POWER_CLASS_8BIT_SHIFT 4
#define EXT_CSD_POWER_CLASS_4BIT_MASK 0x0f
#define EXT_CSD_POWER_CLASS_4BIT_SHIFT 0

/* R4 responses (SDIO) */
#define R4_IO_NUM_FUNCTIONS(ocr) (((ocr) >> 28) & 0x3)
#define R4_IO_MEM_PRESENT (1u << 27)
#define R4_IO_OCR_MASK 0x00fffff0

/* Specifications require 400 kHz max. during ID phase. */
#define SD_MMC_CARD_ID_FREQUENCY 400000

/*
 * SDIO Direct & Extended I/O
 */
#define SD_IO_RW_WR (1u << 31)
#define SD_IO_RW_FUNC(x) (((x) & 0x7) << 28)
#define SD_IO_RW_RAW (1u << 27)
#define SD_IO_RW_INCR (1u << 26)
#define SD_IO_RW_ADR(x) (((x) & 0x1FFFF) << 9)
#define SD_IO_RW_DAT(x) (((x) & 0xFF) << 0)
#define SD_IO_RW_LEN(x) (((x) & 0xFF) << 0)

#define SD_IOE_RW_LEN(x) (((x) & 0x1FF) << 0)
#define SD_IOE_RW_ADR(x) (((x) & 0x1FFFF) << 9)
#define SD_IOE_RW_INCR (1u << 26)
#define SD_IOE_RW_BLK (1u << 27)
#define SD_IOE_RW_FUNC(x) (((x) & 0x7) << 28)
#define SD_IOE_RW_WR (1u << 31)

/* Card Common Control Registers (CCCR) */
#define SD_IO_CCCR_START 0x00000    /* Offset in F0 address space */
#define SD_IO_CCCR_SIZE 0x100       /* Total size of CCCR */
#define SD_IO_CCCR_FN_ENABLE 0x02   /* Enabled functions */
#define SD_IO_CCCR_FN_READY 0x03    /* Function ready status */
#define SD_IO_CCCR_INT_ENABLE 0x04  /* Per-function interrupt enable */
#define SD_IO_CCCR_INT_PENDING 0x05 /* Per-function interrupt pending */
#define SD_IO_CCCR_CTL 0x06         /* I/O Abort register */
#define CCCR_CTL_RES (1u << 3)      /* Perform SDIO reset */
#define SD_IO_CCCR_BUS_WIDTH 0x07   /* Bus Width register */
#define CCCR_BUS_WIDTH_4 (1 << 1)
#define CCCR_BUS_WIDTH_1 (1 << 0)
#define SD_IO_CCCR_CARDCAP 0x08 /* SDIO card capabilities */
#define CCCR_CC_SMB (1 << 1)    /* CMD53 block mode support */
#define CCCR_CC_LSC (1 << 6)

#define SD_IO_CCCR_CISPTR 0x09    /* XXX 9-10, 10-11, or 9-12 */
#define SD_IO_CCCR_FN0_BLKSZ 0x10 /* 0x10 - 0x11 */
#define SD_IO_CCCR_SPEED 0x13
#define CCCR_SPEED_SHS (1 << 0)
#define CCCR_SPEED_BSS_MASK (0x7 << 1)
#define CCCR_SPEED_EHS (1 << 1)
#define CCCR_SPEED_SDR12 (0 << 1)
#define CCCR_SPEED_SDR25 (1 << 1)
#define CCCR_SPEED_SDR50 (2 << 1)
#define CCCR_SPEED_SDR104 (3 << 1)
#define CCCR_SPEED_DDR50 (4 << 1)

/* Card Information Structure (CIS) */
#define SD_IO_CIS_START 0x01000 /* Offset in F0 address space */
#define SD_IO_CIS_SIZE 0x17000  /* Total size of CIS */

/* CIS tuple codes (based on PC Card 16) */
#define SD_IO_CISTPL_VERS_1 0x15
#define SD_IO_CISTPL_MANFID 0x20
#define SD_IO_CISTPL_FUNCID 0x21
#define SD_IO_CISTPL_FUNCE 0x22
#define SD_IO_CISTPL_END 0xff

/* CISTPL_FUNCID codes */
/* OpenBSD incorrectly defines 0x0c as FUNCTION_WLAN */

/* OCR bits */
#define MMC_OCR_VOLTAGE 0x3fffffffU   /* Vdd Voltage mask */
#define MMC_OCR_ABNORMAL_MASK 0x7FU   /* Below Voltage mask */
#define MMC_OCR_LOW_VOLTAGE (1u << 7) /* Low Voltage Range -- tbd */
#define MMC_OCR_MIN_VOLTAGE_SHIFT 7
#define MMC_OCR_200_210 (1U << 8)  /* Vdd voltage 2.00 ~ 2.10 */
#define MMC_OCR_210_220 (1U << 9)  /* Vdd voltage 2.10 ~ 2.20 */
#define MMC_OCR_220_230 (1U << 10) /* Vdd voltage 2.20 ~ 2.30 */
#define MMC_OCR_230_240 (1U << 11) /* Vdd voltage 2.30 ~ 2.40 */
#define MMC_OCR_240_250 (1U << 12) /* Vdd voltage 2.40 ~ 2.50 */
#define MMC_OCR_250_260 (1U << 13) /* Vdd voltage 2.50 ~ 2.60 */
#define MMC_OCR_260_270 (1U << 14) /* Vdd voltage 2.60 ~ 2.70 */
#define MMC_OCR_270_280 (1U << 15) /* Vdd voltage 2.70 ~ 2.80 */
#define MMC_OCR_280_290 (1U << 16) /* Vdd voltage 2.80 ~ 2.90 */
#define MMC_OCR_290_300 (1U << 17) /* Vdd voltage 2.90 ~ 3.00 */
#define MMC_OCR_300_310 (1U << 18) /* Vdd voltage 3.00 ~ 3.10 */
#define MMC_OCR_310_320 (1U << 19) /* Vdd voltage 3.10 ~ 3.20 */
#define MMC_OCR_320_330 (1U << 20) /* Vdd voltage 3.20 ~ 3.30 */
#define MMC_OCR_330_340 (1U << 21) /* Vdd voltage 3.30 ~ 3.40 */
#define MMC_OCR_340_350 (1U << 22) /* Vdd voltage 3.40 ~ 3.50 */
#define MMC_OCR_350_360 (1U << 23) /* Vdd voltage 3.50 ~ 3.60 */
#define MMC_OCR_MAX_VOLTAGE_SHIFT 23
#define MMC_OCR_S18R (1U << 24)             /* Switching to 1.8 V requested (SD) */
#define MMC_OCR_S18A MMC_OCR_S18R           /* Switching to 1.8 V accepted (SD) */
#define MMC_OCR_XPC (1U << 28)              /* SDXC Power Control */
#define MMC_OCR_ACCESS_MODE_BYTE (0U << 29) /* Access Mode Byte (MMC) */
#define MMC_OCR_ACCESS_MODE_SECT (1U << 29) /* Access Mode Sector (MMC) */
#define MMC_OCR_ACCESS_MODE_MASK (3U << 29)
#define MMC_OCR_CCS (1u << 30)       /* Card Capacity status (SD vs SDHC) */
#define MMC_OCR_CARD_BUSY (1U << 31) /* Card Power up status */

#define R5_COM_CRC_ERROR (1u << 15)         /* er, b */
#define R5_ILLEGAL_COMMAND (1u << 14)       /* er, b */
#define R5_IO_CURRENT_STATE_MASK (3u << 12) /* s, b */
#define R5_IO_CURRENT_STATE(x) (((x)&R5_IO_CURRENT_STATE_MASK) >> 12)
#define R5_ERROR (1u << 11)          /* erx, c */
#define R5_FUNCTION_NUMBER (1u << 9) /* er, c */
#define R5_OUT_OF_RANGE (1u << 8)    /* er, c */

#define EXT_CSD_HS_TIMING_BC 0
#define EXT_CSD_HS_TIMING_HS 1
#define EXT_CSD_HS_TIMING_HS200 2
#define EXT_CSD_HS_TIMING_HS400 3
#define EXT_CSD_HS_TIMING_DRV_STR_SHIFT 4

#define IO_SEND_OP_COND 5
#define SD_IO_RW_DIRECT 52
#define SD_IO_RW_EXTENDED 53

#define R5_RESP_CHECK (R5_ERROR | R5_FUNCTION_NUMBER | R5_OUT_OF_RANGE)

/* cccr/fbr version */
#define CCCR_REV_1_00 0x00      /* 1.00 */
#define CCCR_REV_1_10 0x01      /* 1.10 */
#define CCCR_REV_2_00 0x02      /* 2.00 */
#define CCCR_REV_3_00 0x03      /* 3.00 */
#define CCCR_SDIO_REV_1_00 0x00 /* 1.00 */
#define CCCR_SDIO_REV_1_10 0x01 /* 1.10 */
#define CCCR_SDIO_REV_1_20 0x02 /* 1.20 */
#define CCCR_SDIO_REV_2_00 0x03 /* 2.00 */
#define CCCR_SDIO_REV_3_00 0x04 /* 3.00 */
#define CCCR_INT_FUNC_EN_MASK 0xFE
#define CCCR_INT_EN_MASTER 0x01
#define CCCR_BUS_WIDTH_MASK 0x03
#define CCCR_BUS_WIDTH_8 (3u << 0) /* only for embedded sdio */
#define CCCR_CD_DISABLE 0x80

#define CCCR_CC_4BLS (1u << 7)

#define SD_IO_CCCR_PWR_CTL 0x12
/* POWER CONTROL */
#define CCCR_POWER_SMPC (1u << 0)
#define SD_IO_CCCR_SPEED_SEL 0x13

/* Function Basic Registers (FBR) */
#define SD_IO_FBR_EXT_IF 0x01
#define SD_IO_FBR_F_SIZE 0x00100 /* Size of each function */
#define SD_IO_FBR_CIS_OFFSET 0x9 /* Offset of this function's info block within CIS area */
#define SD_IO_FBR_IOBLKSZ 0x10   /* Block size for CMD53 block mode operations */
#define SD_IO_FBR_CSA 0x0C
/* CIS tuple codes (based on PC Card 16) */
#define SD_IO_CISTPL_NULL 0x00
/* ivar->sdio_cccr_caps self_defined */
#define SDIO_CCCR_MULTI_BLK (1u << 0) /* the card's ability to exec CMD53 in the block mode */
#define SDIO_CCCR_HI_POWER (1u << 1)
#define SD_HS_MAX 50000000
#define MAX_TPL_NUM 20

#define MMC_CAP_4_BIT_DATA (1 << 0)      /* Can do 4-bit data transfers */
#define MMC_CAP_8_BIT_DATA (1 << 1)      /* Can do 8-bit data transfers */
#define MMC_CAP_HSPEED (1 << 2)          /* Can do High Speed transfers */
#define MMC_CAP_BOOT_NOACC (1 << 4)      /* Cannot access boot partitions */
#define MMC_CAP_WAIT_WHILE_BUSY (1 << 5) /* Host waits for busy responses */
#define MMC_CAP_UHS_SDR12 (1 << 6)       /* Can do UHS SDR12 */
#define MMC_CAP_UHS_SDR25 (1 << 7)       /* Can do UHS SDR25 */
#define MMC_CAP_UHS_SDR50 (1 << 8)       /* Can do UHS SDR50 */
#define MMC_CAP_UHS_SDR104 (1 << 9)      /* Can do UHS SDR104 */
#define MMC_CAP_UHS_DDR50 (1 << 10)      /* Can do UHS DDR50 */
#define MMC_CAP_MMC_DDR52_120 (1 << 11)  /* Can do eMMC DDR52 at 1.2 V */
#define MMC_CAP_MMC_DDR52_180 (1 << 12)  /* Can do eMMC DDR52 at 1.8 V */
#define MMC_CAP_MMC_DDR52 (MMC_CAP_MMC_DDR52_120 | MMC_CAP_MMC_DDR52_180)
#define MMC_CAP_MMC_HS200_120 (1 << 13) /* Can do eMMC HS200 at 1.2 V */
#define MMC_CAP_MMC_HS200_180 (1 << 14) /* Can do eMMC HS200 at 1.8 V */
#define MMC_CAP_MMC_HS200 (MMC_CAP_MMC_HS200_120 | MMC_CAP_MMC_HS200_180)
#define MMC_CAP_MMC_HS400_120 (1 << 15) /* Can do eMMC HS400 at 1.2 V */
#define MMC_CAP_MMC_HS400_180 (1 << 16) /* Can do eMMC HS400 at 1.8 V */
#define MMC_CAP_MMC_HS400 (MMC_CAP_MMC_HS400_120 | MMC_CAP_MMC_HS400_180)
#define MMC_CAP_MMC_HSX00_120 (MMC_CAP_MMC_HS200_120 | MMC_CAP_MMC_HS400_120)
#define MMC_CAP_MMC_ENH_STROBE (1 << 17) /* Can do eMMC Enhanced Strobe */
#define MMC_CAP_SIGNALING_120 (1 << 18)  /* Can do signaling at 1.2 V */
#define MMC_CAP_SIGNALING_180 (1 << 19)  /* Can do signaling at 1.8 V */
#define MMC_CAP_SIGNALING_330 (1 << 20)  /* Can do signaling at 3.3 V */
#define MMC_CAP_DRIVER_TYPE_A (1 << 21)  /* Can do Driver Type A */
#define MMC_CAP_DRIVER_TYPE_C (1 << 22)  /* Can do Driver Type C */
#define MMC_CAP_DRIVER_TYPE_D (1 << 23)  /* Can do Driver Type D */

#define MMC_CAP_MAX_CURRENT_200 (1 << 24) /* Max current limit 200mA */
#define MMC_CAP_MAX_CURRENT_400 (1 << 25) /* Max current limit 400mA */
#define MMC_CAP_MAX_CURRENT_600 (1 << 26) /* Max current limit 600mA */
#define MMC_CAP_MAX_CURRENT_800 (1 << 27) /* Max current limit 800mA */

#define EXT_CSD_CMD_SET_NORMAL 1
#define EXT_CSD_CMD_SET_SECURE 2
#define EXT_CSD_CMD_SET_CPSECURE 4

/*
 * EXT_CSD fields
 */
#define EXT_CSD_EXT_PART_ATTR 52        /* R/W, 2 bytes */
#define EXT_CSD_ENH_START_ADDR 136      /* R/W, 4 bytes */
#define EXT_CSD_ENH_SIZE_MULT 140       /* R/W, 3 bytes */
#define EXT_CSD_GP_SIZE_MULT 143        /* R/W, 12 bytes */
#define EXT_CSD_PART_SET 155            /* R/W */
#define EXT_CSD_PART_ATTR 156           /* R/W */
#define EXT_CSD_PART_SUPPORT 160        /* RO */
#define EXT_CSD_RPMB_MULT 168           /* RO */
#define EXT_CSD_BOOT_WP_STATUS 174      /* RO */
#define EXT_CSD_ERASE_GRP_DEF 175       /* R/W */
#define EXT_CSD_PART_CONFIG 179         /* R/W */
#define EXT_CSD_BUS_WIDTH 183           /* R/W */
#define EXT_CSD_STROBE_SUPPORT 184      /* RO */
#define EXT_CSD_HS_TIMING 185           /* R/W */
#define EXT_CSD_POWER_CLASS 187         /* R/W */
#define EXT_CSD_CARD_TYPE 196           /* RO */
#define EXT_CSD_DRIVER_STRENGTH 197     /* RO */
#define EXT_CSD_REV 192                 /* RO */
#define EXT_CSD_PART_SWITCH_TO 199      /* RO */
#define EXT_CSD_PWR_CL_52_195 200       /* RO */
#define EXT_CSD_PWR_CL_26_195 201       /* RO */
#define EXT_CSD_PWR_CL_52_360 202       /* RO */
#define EXT_CSD_PWR_CL_26_360 203       /* RO */
#define EXT_CSD_SEC_CNT 212             /* RO, 4 bytes */
#define EXT_CSD_HC_WP_GRP_SIZE 221      /* RO */
#define EXT_CSD_ERASE_TO_MULT 223       /* RO */
#define EXT_CSD_ERASE_GRP_SIZE 224      /* RO */
#define EXT_CSD_BOOT_SIZE_MULT 226      /* RO */
#define EXT_CSD_SEC_FEATURE_SUPPORT 231 /* RO */
#define EXT_CSD_PWR_CL_200_195 236      /* RO */
#define EXT_CSD_PWR_CL_200_360 237      /* RO */
#define EXT_CSD_PWR_CL_52_195_DDR 238   /* RO */
#define EXT_CSD_PWR_CL_52_360_DDR 239   /* RO */
#define EXT_CSD_GEN_CMD6_TIME 248       /* RO */
#define EXT_CSD_PWR_CL_200_360_DDR 253  /* RO */

#define MMC_APP_CMD 55
#define MMC_GEN_CMD 56

/* Application specific commands for SD */
#define ACMD_SET_BUS_WIDTH 6
#define ACMD_SD_STATUS 13
#define ACMD_SEND_NUM_WR_BLOCKS 22
#define ACMD_SET_WR_BLK_ERASE_COUNT 23
#define ACMD_SD_SEND_OP_COND 41
#define ACMD_SET_CLR_CARD_DETECT 42
#define ACMD_SEND_SCR 51

#define EXT_CSD_BUS_WIDTH_1 0
#define EXT_CSD_BUS_WIDTH_4 1
#define EXT_CSD_BUS_WIDTH_8 2
#define EXT_CSD_BUS_WIDTH_4_DDR 5
#define EXT_CSD_BUS_WIDTH_8_DDR 6
#define EXT_CSD_BUS_WIDTH_ES 0x80

#define MMC_PEND_REQUEST_DONE (1U << 0)

#define MMC_VETO_ID 1

#define MMC_ERASE 38
#define MMC_ERASE_ERASE 0x00000000
#define MMC_ERASE_TRIM 0x00000001
#define MMC_ERASE_FULE 0x00000002
#define MMC_ERASE_DISCARD 0x00000003
#define MMC_ERASE_SECURE_ERASE 0x80000000
#define MMC_ERASE_SECURE_TRIM1 0x80000001
#define MMC_ERASE_SECURE_TRIM2 0x80008000

#define MMCSD_INAND_CMD38 0x0001
#define MMCSD_USE_TRIM 0x0002
#define MMCSD_USE_CMD23 4U

#define EXT_CSD_SEC_FEATURE_SUPPORT_ER_EN 0x01
#define EXT_CSD_SEC_FEATURE_SUPPORT_BD_BLK_EN 0x04
#define EXT_CSD_SEC_FEATURE_SUPPORT_GB_CL_EN 0x10
#define EXT_CSD_SEC_FEATURE_SUPPORT_SANITIZE 0x40

#define HISDEMMC_POWER_CONTROL              0x29
#define ARGUMENT                            (EMMC_REG_BASE_ADDESS + 0x8)
#define CMD                                 (EMMC_REG_BASE_ADDESS + 0xE)
#define RESP01                              (EMMC_REG_BASE_ADDESS + 0x10)
#define RESP23                              (EMMC_REG_BASE_ADDESS + 0x14)
#define RESP45                              (EMMC_REG_BASE_ADDESS + 0x18)
#define RESP67                              (EMMC_REG_BASE_ADDESS + 0x1C)
#define PSTATE_REG                          (EMMC_REG_BASE_ADDESS + 0x24)
#define HOST_CTRL1                          (EMMC_REG_BASE_ADDESS + 0x28)
#define HIGH_SPEED_EN_BIT                   0x2
#define EXT_DAT_XFER_BIT                    0x5
#define PWR_CTRL                            (EMMC_REG_BASE_ADDESS + 0x29)
#define POWR_ON_1_8V                        0xBF
#define CLK_CTRL                            (EMMC_REG_BASE_ADDESS + 0x2C)
#define TOUT_CTRL                           (EMMC_REG_BASE_ADDESS + 0x2E)
#define SW_RST                              (EMMC_REG_BASE_ADDESS + 0x2F)
#define SW_RST_ALL                          BIT(0)
#define SW_RST_CMD                          BIT(1)
#define SW_RST_DAT                          BIT(2)
#define SW_RE_RST_ALL                       (SW_RST_ALL | SW_RST_CMD | SW_RST_DAT)
#define NORMAL_INT_STAT                     (EMMC_REG_BASE_ADDESS + 0x30)
#define ERROR_INT_STAT                      (EMMC_REG_BASE_ADDESS + 0x32)
#define ALL_NORMAL_ERR_INT                  0xffff
#define NORMAL_INT_STAT_EN                  (EMMC_REG_BASE_ADDESS + 0x34)
#define ERROR_INT_STAT_EN                   (EMMC_REG_BASE_ADDESS + 0x36)
#define NORMAL_ERR_INT_EN                   0x7EFF
#define NORMAL_INT_SIGNAL_EN                (EMMC_REG_BASE_ADDESS + 0x38)
#define ERROR_INT_SIGNAL_EN                 (EMMC_REG_BASE_ADDESS + 0x3A)
#define HOST_CTRL2                          (EMMC_REG_BASE_ADDESS + 0x3E)
#define SAMPLE_CLK_SEL_BIT                  0x7
#define SIGNALING_EN_BIT                    0x3
#define UHS_MODE_SEL_BIT                    0x0
#define UHS_MODE_SEL_LEN                    0x3
#define HS_SDR                              0x1
#define HS_DDR                              0x4
#define HS200                               0x3
#define HS400                               0x7
#define MBIIU_CTRL                          (EMMC_REG_BASE_ADDESS + DWC_MSHC_PTR_VENDOR1 + 0x10)
#define AT_CTRL                             (EMMC_REG_BASE_ADDESS + DWC_MSHC_PTR_VENDOR1 + 0x40)
#define AT_STAT                             (EMMC_REG_BASE_ADDESS + DWC_MSHC_PTR_VENDOR1 + 0x44)
#define EMMC_CTRL                           (EMMC_REG_BASE_ADDESS + DWC_MSHC_PTR_VENDOR1 + 0x2C)
#define LEN_3BIT                            0x3
#define LEN_8BIT                            0x8

#define ATOMIC_INIT(x) (x)

#define mmc_device_alloc() (device_t) zalloc(sizeof(struct device))
#define mmc_device_free(addr) free(addr)

#ifndef CTZ
#define CTZ(value) (__builtin_ctz(value))
#endif

#define LOS_INVALID_BIT_INDEX 32

#define READ_IVAR(d, c, i, r) (((hci_ops *)get_mmcbus_ops(d))->read_ivar(d, c, i, r))
#define WRITE_IVAR(d, c, i, r) (((hci_ops *)get_mmcbus_ops(d))->write_ivar(d, c, i, r))
#define MMCBR_REQUEST(b, d, r) (((hci_ops *)get_mmcbus_ops(b))->request(b, d, r))
#define MMCBR_HNDL_ERR(b, d) (((hci_ops *)get_mmcbus_ops(b))->handle_error(b, d))
#define MMCBR_GET_HOST_ID(b, d) (((hci_ops *)get_mmcbus_ops(b))->get_host_id(d))

#define MMCBR_ACQUIRE_HOST(b, r) (((hci_ops *)get_mmcbus_ops(b))->acquire_host(b, r))
#define MMCBR_RELEASE_HOST(b, r) (((hci_ops *)get_mmcbus_ops(b))->release_host(b, r))

#define MMCBR_EN_SDIO_IRQ(b, d) (((hci_ops *)get_mmcbus_ops(b))->enable_sdio_irq(b))
#define MMCBR_REG_SDIO_IRQ(b, c, a) (((hci_ops *)get_mmcbus_ops(b))->registor_sdio_irq(b, c, a))

static __inline int MMCBUS_READ_IVAR(device_t dev, device_t child, int index, uintptr_t *r)
{
    mmcbus_ops *ops = get_mmcbus_ops(dev);
    return (ops->read_ivar(dev, child, index, r));
}

static __inline int MMCBUS_WRITE_IVAR(device_t dev, device_t child, int index, uintptr_t v)
{
    mmcbus_ops *ops = get_mmcbus_ops(dev);
    return (ops->write_ivar(dev, child, index, v));
}

static __inline void MMCBUS_RETUNE_PAUSE(device_t busdev, device_t reqdev, bool retune)
{
    mmcbus_ops *ops = get_mmcbus_ops(busdev);
    ops->retune_pause(busdev, reqdev, retune);
}

static __inline void MMCBUS_RETUNE_UNPAUSE(device_t busdev, device_t reqdev)
{
    mmcbus_ops *ops = get_mmcbus_ops(busdev);
    ops->retune_unpause(busdev, reqdev);
}

static __inline int MMCBUS_WAIT_FOR_REQUEST(device_t busdev, device_t reqdev, struct mmc_request *req)
{
    mmcbus_ops *ops = get_mmcbus_ops(busdev);
    return (ops->wait_for_request(busdev, reqdev, req));
}

static __inline void MMCBUS_ACQUIRE_BUS(device_t busdev, device_t reqdev)
{
    mmcbus_ops *ops = get_mmcbus_ops(busdev);
    ops->acquire_bus(busdev, reqdev);
}

static __inline void MMCBUS_RELEASE_BUS(device_t busdev, device_t reqdev)
{
    mmcbus_ops *ops = get_mmcbus_ops(busdev);
    ops->release_bus(busdev, reqdev);
}

static inline device_t device_get_parent(device_t dev)
{
    return (dev ? dev->platform_data : NULL);
}

static inline void device_set_parent(device_t dev, device_t parent)
{
    if (dev == NULL) {
        return;
    }
    dev->platform_data = parent;
}

static __inline int mmcbr_get_bus_mode(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_BUS_MODE, &v);
    return ((int)v);
}

static __inline void mmcbr_set_bus_mode(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_BUS_MODE, v);
}

static __inline int mmcbr_get_bus_width(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_BUS_WIDTH, &v);
    return ((int)v);
}

static __inline void mmcbr_set_bus_width(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_BUS_WIDTH, v);
}

static __inline int mmcbr_get_chip_select(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_CHIP_SELECT, &v);
    return ((int)v);
}

static __inline void mmcbr_set_chip_select(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_CHIP_SELECT, v);
}

static __inline int mmcbr_get_clock(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_CLOCK, &v);
    return ((int)v);
}

static __inline void mmcbr_set_clock(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_CLOCK, v);
}

static __inline int mmcbr_get_f_max(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_F_MAX, &v);
    return ((int)v);
}

static __inline void mmcbr_set_f_max(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_F_MAX, v);
}

static __inline int mmcbr_get_f_min(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_F_MIN, &v);
    return ((int)v);
}

static __inline void mmcbr_set_f_min(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_F_MIN, v);
}
static __inline int mmcbr_get_host_ocr(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_HOST_OCR, &v);
    return ((int)v);
}

static __inline void mmcbr_set_host_ocr(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_HOST_OCR, v);
}

static __inline int mmcbr_get_mode(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_MODE, &v);
    return ((int)v);
}

static __inline void mmcbr_set_mode(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_MODE, v);
}
static __inline unsigned int mmcbr_get_ocr(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_OCR, &v);
    return ((int)v);
}

static __inline void mmcbr_set_ocr(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_OCR, v);
}
static __inline int mmcbr_get_power_mode(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_POWER_MODE, &v);
    return ((int)v);
}

static __inline void mmcbr_set_power_mode(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_POWER_MODE, v);
}

static __inline int mmcbr_get_retune_req(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_RETUNE_REQ, &v);
    return ((int)v);
}

static __inline int mmcbr_get_vdd(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_VDD, &v);
    return ((int)v);
}

static __inline void mmcbr_set_vdd(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_VDD, v);
}

static __inline int mmcbr_get_vccq(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_VCCQ, &v);
    return ((int)v);
}

static __inline void mmcbr_set_vccq(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_VCCQ, v);
}

static __inline unsigned int mmcbr_get_caps(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_CAPS, &v);
    return ((unsigned int)v);
}

static __inline void mmcbr_set_caps(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_CAPS, v);
}
static __inline int mmcbr_get_timing(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_TIMING, &v);
    return ((int)v);
}

static __inline void mmcbr_set_timing(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_TIMING, v);
}

static __inline int mmcbr_get_max_data(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_MAX_DATA, &v);
    return ((int)v);
}

static __inline unsigned int mmcbr_get_max_busy_timeout(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_MAX_BUSY_TIMEOUT, &v);
    return ((unsigned int)v);
}

static __inline void mmcbr_set_max_busy_timeout(device_t dev, unsigned int t)
{
    uintptr_t v = (uintptr_t)t;
    WRITE_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_MAX_BUSY_TIMEOUT, v);
}

static int __inline mmcbr_update_ios(device_t dev)
{
    hci_ops *ops = get_mmcbus_ops(device_get_parent(dev));
    ops->update_ios(device_get_parent(dev), dev);
    return (0);
}

static int __inline mmcbr_tune(device_t dev)
{
    hci_ops *ops = get_mmcbus_ops(device_get_parent(dev));
    return (ops->tune(device_get_parent(dev), dev));
}

static int __inline mmcbr_retune(device_t dev, unsigned int reset)
{
    hci_ops *ops = get_mmcbus_ops(device_get_parent(dev));
    return (ops->retune(device_get_parent(dev), dev, reset));
}

static int __inline mmcbr_switch_vccq(device_t dev)
{
    hci_ops *ops = get_mmcbus_ops(device_get_parent(dev));
    if (ops->switch_vccq == NULL) {
        return (0); /* No need to switch. */
    }
    return (ops->switch_vccq(device_get_parent(dev), dev));
}

static unsigned int __inline mmcbr_get_card_busy(device_t dev)
{
    device_t slot_dev = device_get_parent(dev);
    hci_ops *ops = get_mmcbus_ops(slot_dev);
    if (ops->get_card_busy == NULL) {
        return (1); /* Show card is busy. */
    }
    return (ops->get_card_busy(slot_dev, dev));
}

static int __inline mmcbr_get_ro(device_t dev)
{
    hci_ops *ops = get_mmcbus_ops(device_get_parent(dev));
    return (ops->get_ro(device_get_parent(dev), dev));
}

static unsigned int __inline mmcbr_get_card_present(device_t dev)
{
    return (slot_get_card_present(device_get_parent(dev)));
}

static __inline void mmcbr_set_card_present(device_t dev, unsigned int t)
{
    slot_set_card_present(device_get_parent(dev), t);
}

static __inline unsigned int mmcbr_get_dma_alignment(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_DMA_ALIGNMENT, &v);
    return ((unsigned int)v);
}

static __inline unsigned int mmcbr_get_card_ec(device_t dev)
{
    uintptr_t v;
    READ_IVAR(device_get_parent(dev), dev, MMCBR_IVAR_CARD_ERROR_CNT, &v);
    return ((unsigned int)v);
}

static __inline void mmcbr_enable_sdio_irq(device_t dev)
{
    MMCBR_EN_SDIO_IRQ(device_get_parent(dev), dev);
}

static __inline int mmcbr_registor_sdio_irq(device_t dev, sdio_irq_cb cb, void *arg)
{
    return (MMCBR_REG_SDIO_IRQ(device_get_parent(dev), cb, arg));
}

static inline device_t mmcdev_get_child(device_t dev)
{
    mmcdev_priv *priv = (mmcdev_priv *)mmcdev_get_private_data(dev);
    if (priv == NULL) {
        return (NULL);
    }
    return (priv->child);
}

static inline void mmcdev_set_child(device_t dev, device_t child)
{
    mmcdev_priv *priv = (mmcdev_priv *)mmcdev_get_private_data(dev);
    if (priv == NULL) {
        return;
    }
    priv->child = child;
}

static __inline int mmc_get_rca(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_RCA, &v);
    return ((int)v);
}

static __inline void mmc_set_card_present(device_t dev, unsigned int t)
{
    uintptr_t v = (uintptr_t)t;
    MMCBUS_WRITE_IVAR(device_get_parent(dev), dev, MMC_IVAR_CARD_PRESENT, v);
}

static __inline int mmc_get_card_type(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_CARD_TYPE, &v);
    return ((int)v);
}

static __inline int mmc_get_max_data(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_MAX_DATA, &v);
    return ((int)v);
}

static __inline void mmc_set_max_data(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    MMCBUS_WRITE_IVAR(device_get_parent(dev), dev, MMC_IVAR_MAX_DATA, v);
}

static __inline int mmc_get_high_cap(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_HIGH_CAP, &v);
    return ((int)v);
}

static __inline void mmc_set_high_cap(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    MMCBUS_WRITE_IVAR(device_get_parent(dev), dev, MMC_IVAR_HIGH_CAP, v);
}

static __inline unsigned int mmc_get_cmd6_timeout(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_CMD6_TIMEOUT, &v);
    return ((unsigned int)v);
}

static __inline void mmc_set_cmd6_timeout(device_t dev, unsigned int t)
{
    uintptr_t v = (uintptr_t)t;
    MMCBUS_WRITE_IVAR(device_get_parent(dev), dev, MMC_IVAR_CMD6_TIMEOUT, v);
}

static __inline unsigned int mmc_get_quirks(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_QUIRKS, &v);
    return ((unsigned int)v);
}

static __inline void mmc_set_quirks(device_t dev, unsigned int t)
{
    uintptr_t v = (uintptr_t)t;
    MMCBUS_WRITE_IVAR(device_get_parent(dev), dev, MMC_IVAR_QUIRKS, v);
}

static __inline uint8_t mmc_get_spec_vers(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_SPEC_VERS, &v);
    return ((uint8_t)v);
}

static __inline void mmc_set_spec_vers(device_t dev, uint8_t t)
{
    uintptr_t v = (uintptr_t)t;
    MMCBUS_WRITE_IVAR(device_get_parent(dev), dev, MMC_IVAR_SPEC_VERS, v);
}

static __inline int mmc_get_erase_sector(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_ERASE_SECTOR, &v);
    return ((int)v);
}

static __inline unsigned int mmc_get_cmd23_cap(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_AUTO_CMD23, &v);
    return ((unsigned int)v);
}

static __inline int mmc_get_sector_size(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_SECTOR_SIZE, &v);
    return ((int)v);
}

static __inline void mmc_set_sector_size(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    MMCBUS_WRITE_IVAR(device_get_parent(dev), dev, MMC_IVAR_SECTOR_SIZE, v);
}

static __inline int mmc_get_read_only(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_READ_ONLY, &v);
    return ((int)v);
}

static __inline void mmc_set_read_only(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    MMCBUS_WRITE_IVAR(device_get_parent(dev), dev, MMC_IVAR_READ_ONLY, v);
}

static __inline uint32_t mmc_get_media_size(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_MEDIA_SIZE, &v);
    return ((uint32_t)v);
}

static __inline void mmc_set_media_size(device_t dev, uint32_t t)
{
    uintptr_t v = (uintptr_t)t;
    MMCBUS_WRITE_IVAR(device_get_parent(dev), dev, MMC_IVAR_MEDIA_SIZE, v);
}

static __inline const char *mmc_get_card_id_string(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_CARD_ID_STRING, &v);
    return ((const char *)v);
}

static __inline int mmc_get_bus_width(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_BUS_WIDTH, &v);
    return ((int)v);
}

static __inline void mmc_set_bus_width(device_t dev, int t)
{
    uintptr_t v = (uintptr_t)t;
    MMCBUS_WRITE_IVAR(device_get_parent(dev), dev, MMC_IVAR_BUS_WIDTH, v);
}

static __inline unsigned int mmc_get_au_size(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_AU_SIZE, &v);
    return ((unsigned int)v);
}

static __inline unsigned int mmc_get_card_present(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_CARD_PRESENT, &v);
    return ((unsigned int)v);
}

static __inline unsigned int mmc_get_cis_manid(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_CIS_MANID, &v);
    return ((unsigned int)v);
}

static __inline unsigned int mmc_get_cis_proid(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_CIS_PROID, &v);
    return ((unsigned int)v);
}

static __inline unsigned int mmc_get_erase_cap(device_t dev)
{
    uintptr_t v;
    MMCBUS_READ_IVAR(device_get_parent(dev), dev, MMC_IVAR_CCC_ERASE, &v);
    return ((unsigned int)v);
}

int mmc_detach(device_t dev);
int mmc_attach(device_t dev);
device_t mmc_new_device(device_t busdev);
void mmc_del_device(device_t busdev);
int mmc_hw_reset(device_t mmcbus, device_t dev);
int mmc_device_del_child(device_t child);
LITE_OS_SEC_TEXT uint32_t LOS_EventRead(PEVENT_CB_S eventCB, uint32_t eventMask, uint32_t mode, uint32_t timeout);
int mmc_send_ext_csd(device_t busdev, device_t dev, uint8_t *rawextcsd);
int mmc_switch(device_t busdev, device_t dev, uint16_t rca, uint8_t set, uint8_t index, uint8_t value,
               unsigned int timeout, bool status);
int emmc_input_check(uint32_t start_sector, uint32_t nsectors, int flags);
#endif