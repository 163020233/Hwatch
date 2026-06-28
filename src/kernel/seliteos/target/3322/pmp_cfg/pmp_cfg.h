/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 3322 pmp cfg header for SELiteOS.
 */
#ifndef PMP_CFG_H
#define PMP_CFG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    /* SELITEOS ITCM regions */
    PMP_INDEX_DRVBOX_RODATA_ADDR = 0,           /* tee addr:__drvbox_rodata_start ~ __drvbox_rodata_end */
    PMP_INDEX_USR_TEXT_ADDR = 1,                /* tee addr:__usr_text_start ~ __usr_text_end */
    PMP_INDEX_USR_RODATA_ADDR = 2,              /* tee addr:__usr_rodata_start ~ __usr_rodata_end */
    PMP_INDEX_DRVBOX_TEXT_ADDR = 3,             /* tee addr:__drvbox_text_start ~ __drvbox_text_end */
    PMP_INDEX_KERNEL_TEXT_RODATA_ADDR = 13,     /* tee addr:__kernel_text_rodata_start ~ __kernel_text_rodata_end */

    /* SELITEOS DTCM regions */
    PMP_INDEX_USRBOX_DATA_ADDR = 4,             /* tee addr:__boxA/B_data_start ~ __boxA/B_data_end */
    PMP_INDEX_ALL_USRBOX_DATA_ADDR = 5,         /* tee addr:__usrbox_data_start ~ __usrbox_data_end */
    PMP_INDEX_DRVBOX_DATA_ADDR = 6,             /* tee addr:__drvbox_data_start ~ __drvbox_data_end */
    PMP_INDEX_USR_RO_DATA_ADDR = 7,             /* tee addr:__usr_readonly_data_start ~ __usr_readonly_data_end */
    PMP_INDEX_USR_DATA_ADDR = 8,                /* tee addr:__usr_data_start ~ __usr_data_end */
    PMP_INDEX_SHARED_DATA_ADDR = 9,             /* ree/tee addr:__shared_data_start ~ __shared_data_end */
    PMP_INDEX_KERNEL_DATA_ADDR = 14,            /* tee addr:__kernel_data_start ~ __kernel_data_end */

    /* SELITEOS flash regions */
    PMP_INDEX_USRBOX_TEXT_RODATA_ADDR = 10,     /* tee addr:__boxA/B_text_rodata_start ~ __boxA/B_text_rodata_end */

#ifdef SELITEOS_KERNEL_TEST
    /* flash commonlib text region, for running tee testcase */
    PMP_INDEX_FLASH_COMMON_TEXT_ADDR = 11,      /* tee addr:__flash_common_text_start ~ end */
    /* flash commonlib rodata region, for running tee testcase */
    PMP_INDEX_FLASH_COMMON_RODATA_ADDR = 12,    /* tee addr:__flash_common_rodata_start ~ end */
#endif
    /* when tee kernel remote call ree, use this region */
    PMP_INDEX_SHARED_DATA_DYNAMIC_REGION = 11,  /* tee addr */

    PMP_INDEX_TEE_FLASH_REGION_ADDR = 15,       /* tee addr: the whole tee flash region */

    /* whole sfc region */
    PMP_INDEX_SFC_CODE_ADDR = 18,               /* ree addr:0x2600_0000 - 0x2800_0000 */

    /* SOC register region */
    PMP_INDEX_SOC_REG_ADDR = 28,                /* ree addr:0x5000_0000 - 0x6000_0000 */

    /* Reset TEE startup region */
    PMP_INDEX_SYSTEM_RESET_ADDR = 31,           /* tee addr:0x0 - 0x1000 */
} pmp_addr_index_enum_t;

void set_pmp_kernel_default(void);

void set_pmp_usr_default(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PMP_CFG_H */
