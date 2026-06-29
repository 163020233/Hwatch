/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: At WEAR header \n
 *
 */

#ifndef AT_WEAR_H
#define AT_WEAR_H

#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MB_SIZE                 (1024 * 1024)
#define DTCM_RAM_OFF            0x90000
#define REE_DTCM_OFF            0x20000
#define L2RAM_GMMU_START        0x203E0000
#define ONE_SECOND              (1000000)

void los_at_wear_cmd_register(void);


#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of at_WEAR.h */
