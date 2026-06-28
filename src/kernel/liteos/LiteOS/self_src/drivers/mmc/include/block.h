/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: LiteOS MMC Block Header File
 * Author: Huawei LiteOS Team
 * Create: 2019-06-23
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */
#ifndef _DEV_MMC_BLOCK_H
#define _DEV_MMC_BLOCK_H

#include "stdint.h"

#define	MMC_CARDBUS_BLOCK_SIZE	512

extern struct disk_divide_info emmc;

uint32_t mmc_direct_write(uint32_t host_idx, char *buffer, uint32_t start_sector, uint32_t nsectors);
uint32_t mmc_direct_read(uint32_t host_idx, char *buffer, uint32_t start_sector, uint32_t nsectors);
uint32_t emmc_raw_write(char *buffer, uint32_t start_sector, uint32_t nsectors);
uint32_t emmc_raw_read(char *buffer, uint32_t start_sector, uint32_t nsectors);

/*
 * Only can be called in exception or interrupt, ensure that it is not in the mmc logic process
 * before called in interrupt.
 */
uint32_t mmc_read_in_exception(uint32_t host_idx, char *buffer, uint32_t start_sector, uint32_t nsectors);
uint32_t mmc_write_in_exception(uint32_t host_idx, char *buffer, uint32_t start_sector, uint32_t nsectors);
uint32_t mmc_earse_in_exception(uint32_t host_idx, uint32_t start_sector, uint32_t nsectors);
typedef int32_t (*emmc_partition_ruler)(uint64_t);
void emmc_partition_ruler_register(emmc_partition_ruler ruler);

typedef void (*emmc_card_wake_up)(void);
typedef struct {
    emmc_partition_ruler ruler;
    emmc_card_wake_up card_wakeup;
} mmc_block_ops;
void emmc_block_ops_init(mmc_block_ops *block_ops);

#ifdef LOSCFG_DRIVERS_MMC_RPMB
/* RPMB */
#define RPMB_READ_WRITE_COUNTER       0
#define RPMB_SET_KEY                  1
#define RPMB_WRITE_DATA               2
#define RPMB_READ_DATA                3
#define RPMB_PADING_LEN               196
#define RPMB_MAC_KEY_LEN              32
#define RPMB_DATA_LEN                 256
#define RPMB_RAND_LEN                 16
#define RPMB_RESULT_READ_REQUEST      0x5
#define RPMB_SIZE                     512
#pragma pack(push, 1)
typedef struct {
    uint8_t padding[RPMB_PADING_LEN];
    uint8_t mac_key[RPMB_MAC_KEY_LEN];
    uint8_t data[RPMB_DATA_LEN];
    uint8_t rand[RPMB_RAND_LEN];
    uint32_t write_cnt;
    uint16_t address;
    uint16_t block_cnt;
    uint16_t op_result;
    uint16_t req_resp;
} rpmb_data_t;
#pragma pack(pop)
/**
 * @ingroup  rpmb
 * @brief Functions of the RPMB.
 * @attention Depends on LOSCFG_DRIVERS_MMC_RPMB, and the operType must be noted.
 * #define RPMB_READ_WRITE_COUNTER       0
 * #define RPMB_SET_KEY                  1
 * #define RPMB_WRITE_DATA               2
 * #define RPMB_READ_DATA                3
 * @param  index [IN] eMMC ID
 * @param  operType [IN] Indicates the operation type of the RPMB.
 * @param  buf [IN] RPMB frame data sent to the eMMC.
 * @param  buf [OUT] RPMB frame data returned from the eMMC.
 */
int mmc_rpmb_operate(uint32_t index, uint32_t operType, void *buf);
#endif

#ifdef LOSCFG_DRIVERS_MMC_HISDEMMC
/**
 * @ingroup  CMD56
 * @brief Functions of the CMD56.
 * @attention Get eMMC Information
 * @param  index [IN] eMMC ID
 * @param  arg [IN] cmd parameter
 * @param  buffer [OUT] eMMC Information.
 */
int mmc_general_cmd(uint32_t index, uint32_t arg, unsigned char *buffer);
#endif

#endif
