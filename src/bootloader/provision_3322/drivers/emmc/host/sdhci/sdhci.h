/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2008 Alexander Motin <mav@FreeBSD.org>
 * All rights reserved.
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __SDHCI_H__
#define __SDHCI_H__

#include "emmc_type.h"
#include "hci.h"
#include "tcxo.h"

#define R1_OUT_OF_RANGE (1u << 31)        /* erx, c */
#define R1_ADDRESS_ERROR (1u << 30)       /* erx, c */
#define R1_BLOCK_LEN_ERROR (1u << 29)     /* erx, c */
#define R1_ERASE_SEQ_ERROR (1u << 28)     /* er, c */
#define R1_ERASE_PARAM (1u << 27)         /* erx, c */
#define R1_WP_VIOLATION (1u << 26)        /* erx, c */
#define R1_CARD_IS_LOCKED (1u << 25)      /* sx, a */
#define R1_LOCK_UNLOCK_FAILED (1u << 24)  /* erx, c */
#define R1_COM_CRC_ERROR (1u << 23)       /* er, b */
#define R1_ILLEGAL_COMMAND (1u << 22)     /* er, b */
#define R1_CARD_ECC_FAILED (1u << 21)     /* erx, c */
#define R1_CC_ERROR (1u << 20)            /* erx, c */
#define R1_ERROR (1u << 19)               /* erx, c */
#define R1_CSD_OVERWRITE (1u << 16)       /* erx, c */
#define R1_WP_ERASE_SKIP (1u << 15)       /* erx, c */
#define R1_CARD_ECC_DISABLED (1u << 14)   /* sx, a */
#define R1_ERASE_RESET (1u << 13)         /* sr, c */
#define R1_CURRENT_STATE_MASK (0xfu << 9) /* sx, b */
#define R1_READY_FOR_DATA (1u << 8)       /* sx, a */
#define R1_SWITCH_ERROR (1u << 7)         /* sx, c */
#define R1_APP_CMD (1u << 5)              /* sr, c */
#define R1_AKE_SEQ_ERROR (1u << 3)        /* er, c */
#define R1_STATUS(x) ((x) & 0xFFFFE000)
#define R1_CURRENT_STATE(x) (((x) & R1_CURRENT_STATE_MASK) >> 9)
#define R1_STATE_IDLE 0
#define R1_STATE_READY 1
#define R1_STATE_IDENT 2
#define R1_STATE_STBY 3
#define R1_STATE_TRAN 4
#define R1_STATE_DATA 5
#define R1_STATE_RCV 6
#define R1_STATE_PRG 7
#define R1_STATE_DIS 8
#define MMC_CMD_ERRORS \
    (R1_OUT_OF_RANGE | R1_ADDRESS_ERROR | R1_BLOCK_LEN_ERROR | R1_WP_VIOLATION | R1_CC_ERROR | R1_ERROR)
#define EMMC_STATUS_ERRORS 0xFDFFA000

#define SDHCI_COMMAND_FLAGS 0x0E
#define SDHCI_CMD_RESP_NONE 0x00
#define SDHCI_CMD_RESP_LONG 0x01
#define SDHCI_CMD_RESP_SHORT 0x02
#define SDHCI_CMD_RESP_SHORT_BUSY 0x03
#define SDHCI_CMD_RESP_MASK 0x03
#define SDHCI_CMD_CRC 0x08
#define SDHCI_CMD_INDEX 0x10
#define SDHCI_CMD_DATA 0x20
#define SDHCI_CMD_TYPE_NORMAL 0x00
#define SDHCI_CMD_TYPE_SUSPEND 0x40
#define SDHCI_CMD_TYPE_RESUME 0x80
#define SDHCI_CMD_TYPE_ABORT 0xc0
#define SDHCI_CMD_TYPE_MASK 0xc0

/* Command definitions */

/* Class 0 and 1: Basic commands & read stream commands */
#define MMC_GO_IDLE_STATE 0
#define MMC_SEND_OP_COND 1
#define MMC_ALL_SEND_CID 2
#define MMC_SET_RELATIVE_ADDR 3
#define SD_SEND_RELATIVE_ADDR 3
#define MMC_SET_DSR 4
#define MMC_SLEEP_AWAKE 5
#define IO_SEND_OP_COND 5
#define MMC_SWITCH_FUNC 6
#define MMC_SWITCH_FUNC_CMDS 0
#define MMC_SWITCH_FUNC_SET 1
#define MMC_SWITCH_FUNC_CLR 2
#define MMC_SWITCH_FUNC_WR 3
#define MMC_SELECT_CARD 7
#define MMC_DESELECT_CARD 7
#define MMC_SEND_EXT_CSD 8
#define SD_SEND_IF_COND 8
#define MMC_SEND_CSD 9
#define MMC_SEND_CID 10
#define MMC_READ_DAT_UNTIL_STOP 11
#define MMC_SD_SWITCH_VOLTAGE 11
#define MMC_STOP_TRANSMISSION 12
#define MMC_SEND_STATUS 13
#define MMC_BUSTEST_R 14
#define MMC_GO_INACTIVE_STATE 15
#define MMC_BUSTEST_W 19

/* Class 2: Block oriented read commands */
#define MMC_SET_BLOCKLEN 16
#define MMC_READ_SINGLE_BLOCK 17
#define MMC_READ_MULTIPLE_BLOCK 18
#define MMC_SEND_TUNING_BLOCK 19
#define MMC_SEND_TUNING_BLOCK_HS200 21

/* Class 3: Stream write commands */
#define MMC_WRITE_DAT_UNTIL_STOP 20
/* reserved: 22 */

/* Class 4: Block oriented write commands */
#define MMC_SET_BLOCK_COUNT 23
#define MMC_WRITE_BLOCK 24
#define MMC_WRITE_MULTIPLE_BLOCK 25
#define MMC_PROGARM_CID 26
#define MMC_PROGRAM_CSD 27

/* Class 6: Block oriented write protection commands */
#define MMC_SET_WRITE_PROT 28
#define MMC_CLR_WRITE_PROT 29
#define MMC_SEND_WRITE_PROT 30
/* reserved: 31 */

/* Class 5: Erase commands */
#define SD_ERASE_WR_BLK_START 32
#define SD_ERASE_WR_BLK_END 33
/* 34 -- reserved old command */
#define MMC_ERASE_GROUP_START 35
#define MMC_ERASE_GROUP_END 36
/* 37 -- reserved old command */
#define MMC_ERASE 38
#define MMC_ERASE_ERASE 0x00000000
#define MMC_ERASE_TRIM 0x00000001
#define MMC_ERASE_FULE 0x00000002
#define MMC_ERASE_DISCARD 0x00000003
#define MMC_ERASE_SECURE_ERASE 0x80000000
#define MMC_ERASE_SECURE_TRIM1 0x80000001
#define MMC_ERASE_SECURE_TRIM2 0x80008000

/* Class 9: I/O mode commands */
#define MMC_FAST_IO 39
#define MMC_GO_IRQ_STATE 40
/* reserved: 41 */

/* Class 7: Lock card */
#define MMC_LOCK_UNLOCK 42
/* reserved: 43 */
/* reserved: 44 */
/* reserved: 45 */
/* reserved: 46 */
/* reserved: 47 */
/* reserved: 48 */
/* reserved: 49 */
/* reserved: 50 */
/* reserved: 51 */
/* reserved: 54 */

/* Class 8: Application specific commands */
#define MMC_APP_CMD 55
#define MMC_GEN_CMD 56
/* reserved: 57 */
/* reserved: 58 */
/* reserved: 59 */
/* reserved for mfg: 60 */
/* reserved for mfg: 61 */
/* reserved for mfg: 62 */
/* reserved for mfg: 63 */

/* Class 9: I/O cards (sd) */
#define SD_IO_RW_DIRECT 52
/* CMD52 arguments */
#define SD_ARG_CMD52_READ (0 << 31)
#define SD_ARG_CMD52_WRITE (1 << 31)
#define SD_ARG_CMD52_FUNC_SHIFT 28
#define SD_ARG_CMD52_FUNC_MASK 0x7
#define SD_ARG_CMD52_EXCHANGE (1 << 27)
#define SD_ARG_CMD52_REG_SHIFT 9
#define SD_ARG_CMD52_REG_MASK 0x1ffff
#define SD_ARG_CMD52_DATA_SHIFT 0
#define SD_ARG_CMD52_DATA_MASK 0xff
#define SD_R5_DATA(resp) ((resp)[0] & 0xff)

#define SD_IO_RW_EXTENDED 53
/* CMD53 arguments */
#define SD_ARG_CMD53_READ (0 << 31)
#define SD_ARG_CMD53_WRITE (1 << 31)
#define SD_ARG_CMD53_FUNC_SHIFT 28
#define SD_ARG_CMD53_FUNC_MASK 0x7
#define SD_ARG_CMD53_BLOCK_MODE (1 << 27)
#define SD_ARG_CMD53_INCREMENT (1 << 26)
#define SD_ARG_CMD53_REG_SHIFT 9
#define SD_ARG_CMD53_REG_MASK 0x1ffff
#define SD_ARG_CMD53_LENGTH_SHIFT 0
#define SD_ARG_CMD53_LENGTH_MASK 0x1ff
#define SD_ARG_CMD53_LENGTH_MAX 64 /* XXX should be 511? */

/* Class 10: Switch function commands */
#define SD_SWITCH_FUNC 6
/* reserved: 34 */
/* reserved: 35 */
/* reserved: 36 */
/* reserved: 37 */
/* reserved: 50 */
/* reserved: 57 */

/* Application specific commands for SD */
#define ACMD_SET_BUS_WIDTH 6
#define ACMD_SD_STATUS 13
#define ACMD_SEND_NUM_WR_BLOCKS 22
#define ACMD_SET_WR_BLK_ERASE_COUNT 23
#define ACMD_SD_SEND_OP_COND 41
#define ACMD_SET_CLR_CARD_DETECT 42
#define ACMD_SEND_SCR 51

/*
 * Card Command Classes
 */
#define CCC_BASIC (1U << 0)
#define CCC_BLOCK_READ (1U << 2)
#define CCC_BLOCK_WRITE (1U << 4)
#define CCC_ERASE (1U << 5)

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

/*
 * EXT_CSD field definitions
 */
#define EXT_CSD_EXT_PART_ATTR_DEFAULT 0x0
#define EXT_CSD_EXT_PART_ATTR_SYSTEMCODE 0x1
#define EXT_CSD_EXT_PART_ATTR_NPERSISTENT 0x2

#define EXT_CSD_PART_SET_COMPLETED 0x01

#define EXT_CSD_PART_ATTR_ENH_USR 0x01
#define EXT_CSD_PART_ATTR_ENH_GP0 0x02
#define EXT_CSD_PART_ATTR_ENH_GP1 0x04
#define EXT_CSD_PART_ATTR_ENH_GP2 0x08
#define EXT_CSD_PART_ATTR_ENH_GP3 0x10
#define EXT_CSD_PART_ATTR_ENH_MASK 0x1f

#define EXT_CSD_PART_SUPPORT_EN 0x01
#define EXT_CSD_PART_SUPPORT_ENH_ATTR_EN 0x02
#define EXT_CSD_PART_SUPPORT_EXT_ATTR_EN 0x04

#define EXT_CSD_BOOT_WP_STATUS_BOOT0_PWR 0x01
#define EXT_CSD_BOOT_WP_STATUS_BOOT0_PERM 0x02
#define EXT_CSD_BOOT_WP_STATUS_BOOT0_MASK 0x03
#define EXT_CSD_BOOT_WP_STATUS_BOOT1_PWR 0x04
#define EXT_CSD_BOOT_WP_STATUS_BOOT1_PERM 0x08
#define EXT_CSD_BOOT_WP_STATUS_BOOT1_MASK 0x0c

#define EXT_CSD_ERASE_GRP_DEF_EN 0x01

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

#define EXT_CSD_CMD_SET_NORMAL 1
#define EXT_CSD_CMD_SET_SECURE 2
#define EXT_CSD_CMD_SET_CPSECURE 4

#define EXT_CSD_HS_TIMING_BC 0
#define EXT_CSD_HS_TIMING_HS 1
#define EXT_CSD_HS_TIMING_HS200 2
#define EXT_CSD_HS_TIMING_HS400 3
#define EXT_CSD_HS_TIMING_DRV_STR_SHIFT 4

#define EXT_CSD_POWER_CLASS_8BIT_MASK 0xf0
#define EXT_CSD_POWER_CLASS_8BIT_SHIFT 4
#define EXT_CSD_POWER_CLASS_4BIT_MASK 0x0f
#define EXT_CSD_POWER_CLASS_4BIT_SHIFT 0

#define EXT_CSD_CARD_TYPE_HS_26 0x0001
#define EXT_CSD_CARD_TYPE_HS_52 0x0002
#define EXT_CSD_CARD_TYPE_DDR_52_1_8V 0x0004
#define EXT_CSD_CARD_TYPE_DDR_52_1_2V 0x0008
#define EXT_CSD_CARD_TYPE_HS200_1_8V 0x0010
#define EXT_CSD_CARD_TYPE_HS200_1_2V 0x0020
#define EXT_CSD_CARD_TYPE_HS400_1_8V 0x0040
#define EXT_CSD_CARD_TYPE_HS400_1_2V 0x0080

#define EXT_CSD_BUS_WIDTH_1 0
#define EXT_CSD_BUS_WIDTH_4 1
#define EXT_CSD_BUS_WIDTH_8 2
#define EXT_CSD_BUS_WIDTH_4_DDR 5
#define EXT_CSD_BUS_WIDTH_8_DDR 6
#define EXT_CSD_BUS_WIDTH_ES 0x80

#define EXT_CSD_STROBE_SUPPORT_EN 0x01

#define EXT_CSD_SEC_FEATURE_SUPPORT_ER_EN 0x01
#define EXT_CSD_SEC_FEATURE_SUPPORT_BD_BLK_EN 0x04
#define EXT_CSD_SEC_FEATURE_SUPPORT_GB_CL_EN 0x10
#define EXT_CSD_SEC_FEATURE_SUPPORT_SANITIZE 0x40

/*
 * Vendor specific EXT_CSD fields
 */
/* SanDisk iNAND */
#define EXT_CSD_INAND_CMD38 113
#define EXT_CSD_INAND_CMD38_ERASE 0x00
#define EXT_CSD_INAND_CMD38_TRIM 0x01
#define EXT_CSD_INAND_CMD38_SECURE_ERASE 0x80
#define EXT_CSD_INAND_CMD38_SECURE_TRIM1 0x81
#define EXT_CSD_INAND_CMD38_SECURE_TRIM2 0x82

#define MMC_TYPE_HS_26_MAX 26000000
#define MMC_TYPE_HS_52_MAX 52000000
#define MMC_TYPE_DDR52_MAX 52000000
#define MMC_TYPE_HS200_HS400ES_MAX 200000000

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

/* Specifications require 400 kHz max. during ID phase. */
#define SD_MMC_CARD_ID_FREQUENCY 400000
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

/* Function Basic Registers (FBR) */
#define SD_IO_FBR_START 0x00100  /* Offset in F0 address space */
#define SD_IO_FBR_SIZE 0x00700   /* Total size of FBR */
#define SD_IO_FBR_F_SIZE 0x00100 /* Size of each function */
#define SD_IO_FBR_START_F(n) (SD_IO_FBR_START + (n - 1) * SD_IO_FBR_F_SIZE)
#define SD_IO_FBR_CIS_OFFSET 0x9 /* Offset of this function's info block within CIS area */
#define SD_IO_FBR_IOBLKSZ 0x10   /* Block size for CMD53 block mode operations */

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
/* #define	SDMMC_FUNCTION_WLAN		0x0c */

/* OCR bits */

/*
 * in SD 2.0 spec, bits 8-14 are now marked reserved
 * Low voltage in SD2.0 spec is bit 7, TBD voltage
 * Low voltage in MC 3.31 spec is bit 7, 1.65-1.95V
 * Specs prior to  MMC 3.31 defined bits 0-7 as voltages down to 1.5V.
 * 3.31 redefined them to be reserved and also said that cards had to
 * support the 2.7-3.6V and fixed the OCR to be 0xfff8000 for high voltage
 * cards.  MMC 4.0 says that a dual voltage card responds with 0xfff8080.
 * Looks like the fine-grained control of the voltage tolerance ranges
 * was abandoned.
 *
 * The MMC_OCR_CCS appears to be valid for only SD cards.
 */
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

#define SDHCI_TIMEOUT_CONTROL 0x2E

#define SDHCI_USE_SDMA 0x10        /* Can do SDMA */
#define SDHCI_USE_ADMA2 0x20       /* Can do 32-bit ADMA */
#define SDHCI_USE_ADMA2_64BIT 0x40 /* Can do 64-bit ADMA */

#define SDHCI_DMA_ADDRESS 0x00

#define SDHCI_ADMA_HI_SHIFT 32U
#define SDHCI_ADMA_ADDRESS_LO 0x58
#define SDHCI_ADMA_ADDRESS_HI 0x5C

#define SDHCI_HOST_CONTROL 0x28
#define SDHCI_CTRL_LED 0x01
#define SDHCI_CTRL_4BITBUS 0x02
#define SDHCI_CTRL_HISPD 0x04
#define SDHCI_CTRL_SDMA 0x08
#define SDHCI_CTRL_ADMA2 0x10
#define SDHCI_CTRL_ADMA2_64BIT 0x18
#define SDHCI_CTRL_DMA_MASK 0x18
#define SDHCI_CTRL_8BITBUS 0x20
#define SDHCI_CTRL_CARD_DET 0x40
#define SDHCI_CTRL_FORCE_CARD 0x80

#define SDHCI_BLOCK_SIZE 0x04
#define SDHCI_MAKE_BLKSZ(dma, blksz) ((((dma) & 0x7) << 12) | ((blksz) & 0xFFF))

#define DMA_BLOCK_SIZE 4096
#define DMA_BOUNDARY 7 /* DMA reload every 4K */

#define SDHCI_BLOCK_COUNT 0x06

#define SDHCI_ARGUMENT 0x08

#define SDHCI_TRANSFER_MODE 0x0C
#define SDHCI_TRNS_DMA 0x01
#define SDHCI_TRNS_BLK_CNT_EN 0x02
#define SDHCI_TRNS_ACMD12 0x04
#define SDHCI_TRNS_ACMD23 0x08
#define SDHCI_TRNS_READ 0x10
#define SDHCI_TRNS_MULTI 0x20

#define SDHCI_RESPONSE 0x10

#define LOS_WAITMODE_CLR 1U
#define LOS_WAITMODE_OR 2U
#define LOS_WAITMODE_AND 4U

#define LOS_WAIT_FOREVER 0xFFFFFFFF

#define ENOERR 0

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(_x) (sizeof(_x) / sizeof(_x[0]))
#endif

#define MMC_SECTOR_SIZE 512

#define SDHCI_PRESENT_STATE 0x24
#define SDHCI_CMD_INHIBIT 0x00000001
#define SDHCI_DAT_INHIBIT 0x00000002
#define SDHCI_DAT_ACTIVE 0x00000004
#define SDHCI_RETUNE_REQUEST 0x00000008
#define SDHCI_DOING_WRITE 0x00000100
#define SDHCI_DOING_READ 0x00000200
#define SDHCI_SPACE_AVAILABLE 0x00000400
#define SDHCI_DATA_AVAILABLE 0x00000800
#define SDHCI_CARD_PRESENT 0x00010000
#define SDHCI_CARD_STABLE 0x00020000
#define SDHCI_CARD_PIN 0x00040000
#define SDHCI_WRITE_PROTECT 0x00080000
#define SDHCI_STATE_DAT0_MASK 0x00100000
#define SDHCI_STATE_DAT_MASK 0x00f00000
#define SDHCI_STATE_CMD 0x01000000

#define SDHCI_HAVE_DMA 0x01
#define SDHCI_PLATFORM_TRANSFER 0x02
#define SDHCI_NON_REMOVABLE 0x04 /* Used for EMMC/SDIO slot */
#define SDHCI_TUNING_SUPPORTED 0x08
#define SDHCI_TUNING_ENABLED 0x10
#define SDHCI_SDR50_NEEDS_TUNING 0x20
#define SDHCI_SLOT_EMBEDDED 0x40 /* Used for EMMC slot */
#define SDHCI_SLOT_SDIO 0x80

#define LOSCFG_BASE_CORE_TICK_PER_SECOND 1000
#define SDHCI_HZ LOSCFG_BASE_CORE_TICK_PER_SECOND

#define MMC_TUNING_MAX 64        /* Maximum tuning iterations */
#define MMC_TUNING_LEN 64        /* Size of tuning data */
#define MMC_TUNING_LEN_HS200 128 /* Size of tuning data in HS200 mode */

#define SDHCI_INT_STATUS 0x30
#define SDHCI_INT_ENABLE 0x34
#define SDHCI_SIGNAL_ENABLE 0x38
#define SDHCI_INT_RESPONSE 0x00000001
#define SDHCI_INT_DATA_END 0x00000002
#define SDHCI_INT_BLOCK_GAP 0x00000004
#define SDHCI_INT_DMA_END 0x00000008
#define SDHCI_INT_SPACE_AVAIL 0x00000010
#define SDHCI_INT_DATA_AVAIL 0x00000020
#define SDHCI_INT_CARD_INSERT 0x00000040
#define SDHCI_INT_CARD_REMOVE 0x00000080
#define SDHCI_INT_CARD_INT 0x00000100
#define SDHCI_INT_INT_A 0x00000200
#define SDHCI_INT_INT_B 0x00000400
#define SDHCI_INT_INT_C 0x00000800
#define SDHCI_INT_RETUNE 0x00001000
#define SDHCI_INT_ERROR 0x00008000
#define SDHCI_INT_TIMEOUT 0x00010000
#define SDHCI_INT_CRC 0x00020000
#define SDHCI_INT_END_BIT 0x00040000
#define SDHCI_INT_INDEX 0x00080000
#define SDHCI_INT_DATA_TIMEOUT 0x00100000
#define SDHCI_INT_DATA_CRC 0x00200000
#define SDHCI_INT_DATA_END_BIT 0x00400000
#define SDHCI_INT_BUS_POWER 0x00800000
#define SDHCI_INT_ACMD12ERR 0x01000000
#define SDHCI_INT_ADMAERR 0x02000000
#define SDHCI_INT_TUNEERR 0x04000000

#define SDHCI_INT_CMD_ERROR_MASK (SDHCI_INT_TIMEOUT | SDHCI_INT_CRC | SDHCI_INT_END_BIT | SDHCI_INT_INDEX)

#define SDHCI_INT_CMD_MASK (SDHCI_INT_RESPONSE | SDHCI_INT_CMD_ERROR_MASK)

#define SDHCI_INT_DATA_MASK                                                                                           \
    (SDHCI_INT_DATA_END | SDHCI_INT_DMA_END | SDHCI_INT_DATA_AVAIL | SDHCI_INT_SPACE_AVAIL | SDHCI_INT_DATA_TIMEOUT | \
     SDHCI_INT_DATA_CRC | SDHCI_INT_DATA_END_BIT)

#define SDHCI_BLOCK_GAP_CONTROL 0x2A

#define SDHCI_WAKE_UP_CONTROL 0x2B

#define SDHCI_ACMD12_ERR 0x3C

#define SDHCI_SLOT_INT_STATUS 0xFC

#define SDHCI_MAX_CURRENT 0x48
#define SDHCI_MAX_CUR_18V_MASK 0xFF0000U
#define SDHCI_FORCE_AUTO_EVENT 0x50
#define SDHCI_FORCE_INTR_EVENT 0x52

#define SDHCI_ADMA_ERR 0x54
#define SDHCI_ADMA_ERR_LENGTH 0x04
#define SDHCI_ADMA_ERR_STATE_MASK 0x03
#define SDHCI_ADMA_ERR_STATE_STOP 0x00
#define SDHCI_ADMA_ERR_STATE_FDS 0x01
#define SDHCI_ADMA_ERR_STATE_TFR 0x03

#define MMC_RSP_PRESENT (1ul << 0) /* Response */
#define MMC_RSP_136 (1ul << 1)     /* 136 bit response */
#define MMC_RSP_CRC (1ul << 2)     /* Expect valid crc */
#define MMC_RSP_BUSY (1ul << 3)    /* Card may send busy */
#define MMC_RSP_OPCODE (1ul << 4)  /* Response include opcode */
#define MMC_RSP_MASK 0x1ful
#define MMC_CMD_AC (0ul << 5)   /* Addressed Command, no data */
#define MMC_CMD_ADTC (1ul << 5) /* Addressed Data transfer cmd */
#define MMC_CMD_BC (2ul << 5)   /* Broadcast command, no response */
#define MMC_CMD_BCR (3ul << 5)  /* Broadcast command with response */
#define MMC_CMD_MASK (3ul << 5)

/* Possible response types defined in the standard: */
#define MMC_RSP_NONE (0)
#define MMC_RSP_R1 (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE)
#define MMC_RSP_R1B (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE | MMC_RSP_BUSY)
#define MMC_RSP_R2 (MMC_RSP_PRESENT | MMC_RSP_136 | MMC_RSP_CRC)
#define MMC_RSP_R3 (MMC_RSP_PRESENT)
#define MMC_RSP_R4 (MMC_RSP_PRESENT)
#define MMC_RSP_R5 (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE)
#define MMC_RSP_R5B (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE | MMC_RSP_BUSY)
#define MMC_RSP_R6 (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE)
#define MMC_RSP_R7 (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE)
#define MMC_RSP(x) ((x)&MMC_RSP_MASK)

#define MMC_DATA_WRITE (1UL << 0)
#define MMC_DATA_READ (1UL << 1)
#define MMC_DATA_STREAM (1UL << 2)
#define MMC_DATA_MULTI (1UL << 3)
#define MMC_DATA_BLOCK_SIZE (1UL << 4)
#define MMC_DATA_SDIO (1UL << 5)
#define MMC_DATA_SG (1UL << 6)

#define MMC_CAP_CARD_PRESENT (1U << 0)
#define MMC_CAP_CARD_ABSENT (0U << 0)

#define MMC_ERR_NONE 0
#define MMC_ERR_TIMEOUT 1
#define MMC_ERR_BADCRC 2
#define MMC_ERR_FIFO 3
#define MMC_ERR_FAILED 4
#define MMC_ERR_INVALID 5
#define MMC_ERR_NO_MEMORY 6
#define MMC_ERR_ADMA 7
#define MMC_ERR_NO_REP 8
#define MMC_ERR_UNPLUGGED 9
#define MMC_ERR_MAX 10

#define IORESOURCE_MEM 0x00000200
#define IORESOURCE_REG 0x00000300 /* Register offsets */
#define IORESOURCE_IRQ 0x00000400

#define MAX_MMC_NUM 2
#define USE_MMC0 1
#define USE_MMC1 1

#define SDHCI_RETUNE_REQ_NEEDED 0x01 /* Re-tuning w/o circuit reset needed */
#define SDHCI_RETUNE_REQ_RESET 0x02  /* Re-tuning w/ circuit reset needed */

#define SDHCI_RETUNE_MODE_1 0x00
#define SDHCI_RETUNE_MODE_2 0x01
#define SDHCI_RETUNE_MODE_3 0x02

#define SDHCI_HOST_VERSION 0xFE
#define SDHCI_VENDOR_VER_MASK 0xFF00
#define SDHCI_VENDOR_VER_SHIFT 8
#define SDHCI_SPEC_VER_MASK 0x00FF
#define SDHCI_SPEC_VER_SHIFT 0
#define SDHCI_SPEC_100 0
#define SDHCI_SPEC_200 1
#define SDHCI_SPEC_300 2
#define SDHCI_SPEC_400 3
#define SDHCI_SPEC_410 4
#define SDHCI_SPEC_420 5

#define CMD_STARTED 1
#define STOP_STARTED 2
#define SDHCI_USE_DMA 4         /* Use DMA for this req. */
#define PLATFORM_DATA_STARTED 8 /* Data xfer is handled by platform */
#define BC_STARTED 0x10

#define MMC_DATA_SG_MODE(flags) (flags & MMC_DATA_SG)

#define SDHCI_ACQUIRE_HOST 1

#define OS_SCHEDULER_ACTIVE 0  // non-os
#define OS_EXC_ACTIVE 1

#define SDHCI_SOFTWARE_RESET 0x2F
#define SDHCI_RESET_ALL 0x01
#define SDHCI_RESET_CMD 0x02
#define SDHCI_RESET_DATA 0x04

#define MMC_TUNE_DONE 2
#define MMC_REQ_IN_EXC 4 /* request in exception context */

#define SDHCI_TUNINT_REQ_TIMEOUT (LOSCFG_BASE_CORE_TICK_PER_SECOND / 5)

#define LOS_ERRNO_OS_ID (0x00U << 16)
#define LOS_ERRTYPE_ERROR (0x02U << 24)

#define LOS_ERRNO_OS_ERROR(MID, ERRNO) \
    (LOS_ERRTYPE_ERROR | LOS_ERRNO_OS_ID | ((uint32_t)(MID) << 8) | ((uint32_t)(ERRNO)))

#define IRQ_DEFAULT_PRIORITY 0xa0

#define SDHCI_DEFAULT_MAX_FREQ 50

#define OS_SYS_US_PER_SECOND 1000000

#define SDHCI_INT_NORMAL_MASK 0x00007FFF
#define SDHCI_INT_ERROR_MASK 0xFFFF8000

#ifdef LOSCFG_DRIVERS_MMC_RW_IN_BOOT_AND_EXC
#ifndef MMC_PRE_SCAN_COUNT
#define MMC_PRE_SCAN_COUNT (50)
#endif
#ifndef MMC_SCAN_INTERVAL
#define MMC_SCAN_INTERVAL (50 * 1000) /* us */
#endif
#define MMC_RELOAD_DELAY_IN_EXC (1500) /* us */
#endif

/**
 * @ingroup los_errno
 * Define the ID of each module in kernel. The ID is used in error code.
 */
enum LOS_MOUDLE_ID {
    LOS_MOD_SYS = 0x0,         /**< System ID. Its value is 0x0. */
    LOS_MOD_MEM = 0x1,         /**< Dynamic memory module ID. Its value is 0x1. */
    LOS_MOD_TSK = 0x2,         /**< Task module ID. Its value is 0x2. */
    LOS_MOD_SWTMR = 0x3,       /**< Software timer module ID. Its value is 0x3. */
    LOS_MOD_TICK = 0x4,        /**< Tick module ID. Its value is 0x4. */
    LOS_MOD_MSG = 0x5,         /**< Message module ID. Its value is 0x5. */
    LOS_MOD_QUE = 0x6,         /**< Queue module ID. Its value is 0x6. */
    LOS_MOD_SEM = 0x7,         /**< Semaphore module ID. Its value is 0x7. */
    LOS_MOD_MBOX = 0x8,        /**< Static memory module ID. Its value is 0x8. */
    LOS_MOD_HWI = 0x9,         /**< Hardware interrupt module ID. Its value is 0x9. */
    LOS_MOD_HWWDG = 0xa,       /**< Hardware watchdog module ID. Its value is 0xa. */
    LOS_MOD_CACHE = 0xb,       /**< Cache module ID. Its value is 0xb. */
    LOS_MOD_HWTMR = 0xc,       /**< Hardware timer module ID. Its value is 0xc. */
    LOS_MOD_MMU = 0xd,         /**< MMU module ID. Its value is 0xd. */
    LOS_MOD_LOG = 0xe,         /**< Log module ID. Its value is 0xe. */
    LOS_MOD_ERR = 0xf,         /**< Error handling module ID. Its value is 0xf. */
    LOS_MOD_EXC = 0x10,        /**< Exception handling module ID. Its value is 0x10. */
    LOS_MOD_CSTK = 0x11,       /**< This module ID is reserved. Its value is 0x11. */
    LOS_MOD_MPU = 0x12,        /**< MPU module ID. Its value is 0x12. */
    LOS_MOD_NMHWI = 0x13,      /**< NMI module ID. It is reserved. Its value is 0x13. */
    LOS_MOD_TRACE = 0x14,      /**< Trace module ID. Its value is 0x14. */
    LOS_MOD_KNLSTAT = 0x15,    /**< This module ID is reserved. Its value is 0x15. */
    LOS_MOD_EVTTIME = 0x16,    /**< This module ID is reserved. Its value is 0x16. */
    LOS_MOD_THRDCPUP = 0x17,   /**< This module ID is reserved. Its value is 0x17. */
    LOS_MOD_IPC = 0x18,        /**< This module ID is reserved. Its value is 0x18. */
    LOS_MOD_STKMON = 0x19,     /**< This module ID is reserved. Its value is 0x19. */
    LOS_MOD_TIMER = 0x1a,      /**< This module ID is reserved. Its value is 0x1a. */
    LOS_MOD_RESLEAKMON = 0x1b, /**< This module ID is reserved. Its value is 0x1b. */
    LOS_MOD_EVENT = 0x1c,      /**< event module ID. Its value is 0x1c. */
    LOS_MOD_MUX = 0x1d,        /**< mutex module ID. Its value is 0x1d. */
    LOS_MOD_CPUP = 0x1e,       /**< CPU usage module ID. Its value is 0x1e. */
    LOS_MOD_FPB = 0x1f,        /**< FPB module ID. Its value is 0x1f. */
    LOS_MOD_PERF = 0x20,       /**< Perf module ID. Its value is 0x20. */
    LOS_MOD_LPM = 0x21,        /**< LPM module ID. Its value is 0x21. */
    LOS_MOD_RWSEM = 0x22,      /**< RW Semaphore module ID. Its value is 0x22. */
    LOS_MOD_DYN_SHM = 0x23,    /**< dynamic shared memory module ID. Its value is 0x23. */
    LOS_MOD_RPQUEUE = 0x25,    /**< Remote Processor Queue module ID. Its value is 0x25. */
    LOS_MOD_DYNLOAD = 0x26,    /**< Dynload module ID. Its value is 0x26. */
    LOS_MOD_SHELL = 0x31,      /**< shell module ID. Its value is 0x31. */
    LOS_MOD_DRIVER = 0x41,     /**< driver module ID. Its value is 0x41. */
    LOS_MOD_LP = 0x42,         /**< lp module ID. Its value is 0x42. */
    LOS_MOD_BUTT               /**< It is end flag of this enumeration. */
};

/**
 * @ingroup los_event
 * Event reading error code: Event reading times out.
 *
 * Value: 0x02001c01
 *
 * Solution: Increase the waiting time for event reading, or make another task write a mask for the event.
 */
#define LOS_ERRNO_EVENT_READ_TIMEOUT LOS_ERRNO_OS_ERROR(LOS_MOD_EVENT, 0x01)

static inline void *mmcdev_get_private_data(device_t dev)
{
    return (dev ? dev->driver_data : NULL);
}

static inline void mmcdev_set_private_data(device_t dev, void *data)
{
    if (dev == NULL)
        return;
    dev->driver_data = data;
}

static inline int mmcdev_priv_create(device_t dev, void *ops, void *softc, void *ivars)
{
    mmcdev_priv *priv = (mmcdev_priv *)mmcdev_get_private_data(dev);
    if (priv == NULL) {
        priv = zalloc(sizeof(mmcdev_priv));
        if (priv == NULL) {
            return (-ENOMEM);
        }
    }

    priv->softc = softc;
    priv->ivars = ivars;
    priv->ops = ops;
    mmcdev_set_private_data(dev, priv);
    return (ENOERR);
}

static inline void mmcdev_set_ivars(device_t dev, void *ivars)
{
    mmcdev_priv *priv = (mmcdev_priv *)mmcdev_get_private_data(dev);
    if (priv == NULL)
        return;
    priv->ivars = ivars;
}

static inline void *mmcdev_get_ivars(device_t dev)
{
    mmcdev_priv *priv = (mmcdev_priv *)mmcdev_get_private_data(dev);
    if (priv == NULL) {
        return (NULL);
    }
    return (priv->ivars);
}

static inline void *mmcdev_get_softc(device_t dev)
{
    mmcdev_priv *priv = (mmcdev_priv *)mmcdev_get_private_data(dev);
    if (priv == NULL) {
        return (NULL);
    }
    return (priv->softc);
}

static inline void mmcdev_set_softc(device_t dev, void *softc)
{
    mmcdev_priv *priv = (mmcdev_priv *)mmcdev_get_private_data(dev);
    if (priv == NULL) {
        return;
    }
    priv->softc = softc;
}

static inline void mmcdev_priv_destroy(device_t dev)
{
    if (dev == NULL) {
        return;
    }
    mmcdev_priv *priv = (mmcdev_priv *)mmcdev_get_private_data(dev);
    if (priv != NULL) {
        free(priv);
        mmcdev_set_private_data(dev, NULL);
    }
}

static inline void *get_mmcbus_ops(device_t dev)
{
    mmcdev_priv *priv = (mmcdev_priv *)mmcdev_get_private_data(dev);
    if (priv == NULL) {
        return (NULL);
    }
    return (priv->ops);
}

static inline bool LOS_AtomicCmpXchg32bits(Atomic *v, int val, int oldVal)
{
    int prevVal;

    prevVal = *v;
    if (prevVal == oldVal) {
        *v = val;
    }

    return (prevVal != oldVal);
}

static inline void LOS_AtomicDec(Atomic *v)
{
    *v -= 1;
}

static inline void LOS_AtomicInc(Atomic *v)
{
    *v += 1;
}

static inline int LOS_AtomicRead(const Atomic *v)
{
    return *(volatile int *)v;
}

static __inline bool slot_get_card_present(device_t bus)
{
    hci_ops *ops = (hci_ops *)get_mmcbus_ops(bus);
    return (ops->get_card_present(bus));
}

/*
 * This interface would be used when device is not in bus
 * cannot use READ_IVAR because it has 2 parameters, bus and device.
 * that the slot_* interface may not have device in slot.
 */
static __inline void slot_set_card_present(device_t bus, uint32_t p)
{
    hci_ops *ops = (hci_ops *)get_mmcbus_ops(bus);
    ops->set_card_present(bus, p);
}

static __inline bool slot_proc_get_card_attach(device_t bus)
{
    hci_ops *ops = (hci_ops *)get_mmcbus_ops(bus);
    return (ops->proc_get_card_attach(bus));
}

#define RD1(slot, off) (((hci_ops *)get_mmcbus_ops((slot)->bus))->readb)((slot)->bus, (off))
#define RD2(slot, off) (((hci_ops *)get_mmcbus_ops((slot)->bus))->readw)((slot)->bus, (off))
#define RD4(slot, off) (((hci_ops *)get_mmcbus_ops((slot)->bus))->readl)((slot)->bus, (off))

#define WR1(slot, off, val) (((hci_ops *)get_mmcbus_ops((slot)->bus))->writeb)((slot)->bus, (off), (val))
#define WR2(slot, off, val) (((hci_ops *)get_mmcbus_ops((slot)->bus))->writew)((slot)->bus, (off), (val))
#define WR4(slot, off, val) (((hci_ops *)get_mmcbus_ops((slot)->bus))->writel)((slot)->bus, (off), (val))

#define device_set_ivars mmcdev_set_ivars
#define device_get_ivars mmcdev_get_ivars
#define device_set_softc mmcdev_set_softc
#define device_get_softc mmcdev_get_softc

#define SDHCI_RESET sdhci_generic_reset
#define sdhci_dumpcaps sdhci_dumpcaps_buf

#define __predict_false(exp) exp

#define DELAY(t) uapi_tcxo_delay_us(t)
#define ASSERT_DEBUG(exp)                                                          \
    do {                                                                           \
        if ((uintptr_t)(exp) == 0) {                                               \
            PRINT("ASSERT ERROR! %s, %d, %s\n", __FILE__, __LINE__, __FUNCTION__); \
        }                                                                          \
    } while (0)

#define KASSERT(exp, msg) ASSERT_DEBUG(exp)

struct platform_device *get_mmc_device(int id);
int MMC_HostInitById(unsigned int id);

int sdhci_generic_read_ivar(device_t bus, device_t child, int which, uintptr_t *result);
int sdhci_generic_write_ivar(device_t bus, device_t child, int which, uintptr_t value);
int sdhci_init_slot(device_t dev, struct sdhci_slot *slot, int num);
void sdhci_deinit_slot(struct sdhci_slot *slot);
void sdhci_start_slot(struct sdhci_slot *slot);
/* performs generic clean-up for platform transfers */
void sdhci_finish_data(struct sdhci_slot *slot);
int sdhci_cleanup_slot(struct sdhci_slot *slot);
int sdhci_generic_suspend(struct sdhci_slot *slot);
int sdhci_generic_resume(struct sdhci_slot *slot);
void sdhci_generic_reset(device_t brdev, struct sdhci_slot *slot, uint8_t mask);
void sdhci_generic_update_ios(device_t brdev, device_t reqdev);
int sdhci_generic_switch_vccq(device_t brdev, device_t reqdev);
int sdhci_generic_retune(device_t brdev, device_t reqdev, bool reset);
int sdhci_generic_request(device_t brdev, device_t reqdev, struct mmc_request *req);
int sdhci_generic_get_ro(device_t brdev, device_t reqdev);
int sdhci_generic_acquire_host(device_t brdev, device_t reqdev);
int sdhci_generic_release_host(device_t brdev, device_t reqdev);
void sdhci_generic_intr(struct sdhci_slot *slot);
uint32_t sdhci_generic_min_freq(device_t brdev, struct sdhci_slot *slot);
bool sdhci_generic_get_card_present(device_t brdev, struct sdhci_slot *slot);
void sdhci_generic_set_uhs_timing(device_t brdev, struct sdhci_slot *slot);
void sdhci_dump_regs(const struct sdhci_slot *slot);
uint32_t sdhci_get_host_id(device_t reqdev);

void sdhci_start(struct sdhci_slot *slot);

void sdhci_card_detect(void *arg, uint32_t event);
void sdhci_req_wakeup(struct mmc_request *req);

void host_hw_init(void *slot);
uint32_t sdhci_adma_alian_size(const adma2_s *adma2);
struct hci_softc *get_mmc_resource(int index);
void host_set_clock(struct sdhci_slot *slot, uint32_t clock);
void emmc_couter_config_register(config_emmc_router router);
int sent_stop(device_t mmcbus, device_t dev, uint16_t rca, uint32_t write_flag, uint32_t *status);

void set_emmc_capcity_size(uint64_t emmc_capcity_size);
#endif