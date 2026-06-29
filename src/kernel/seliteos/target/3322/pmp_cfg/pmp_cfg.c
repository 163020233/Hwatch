/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: pmp cfg source for SELiteOS.
 */
#include "seliteos_memory_config.h"
#include "pmp_mgr.h"
#include "arch/pmp_lite.h"
#include "pmp_cfg.h"

void set_pmp_kernel_default(void)
{
    /* shared data region 9 */
    pmp_region_config(PMP_INDEX_SHARED_DATA_ADDR, PMP_PERMISSION_U_RW_P_RW, SHARED_DATA_START, SHARED_DATA_SIZE,
        MEM_ATTR_NORM_NON_CA_BUF);

#ifdef SELITEOS_KERNEL_TEST
    /* flash common text region: 11 */
    pmp_region_config(PMP_INDEX_FLASH_COMMON_TEXT_ADDR, PMP_PERMISSION_U_X_P_RX, FLASH_COMMON_TEXT_START,
        FLASH_COMMON_TEXT_SIZE, MEM_ATTR_WB_RD_ALLOC);

    /* flash common rodata region 12 */
    pmp_region_config(PMP_INDEX_FLASH_COMMON_RODATA_ADDR, PMP_PERMISSION_U_R_P_R, FLASH_COMMON_RODATA_START,
        FLASH_COMMON_RODATA_SIZE, MEM_ATTR_WB_RD_ALLOC);
#else
    /* shared data dynamic region: 11 */
    pmp_region_config(PMP_INDEX_SHARED_DATA_DYNAMIC_REGION, PMP_PERMISSION_U_NA_P_RW, SHARED_DATA_TEE_START,
        SHARED_DATA_TEE_ALLOC_SIZE, MEM_ATTR_NORM_NON_CA_BUF);
#endif

    /* kernel text and rodata region 13 */
    pmp_region_config(PMP_INDEX_KERNEL_TEXT_RODATA_ADDR, PMP_PERMISSION_U_NA_P_RX, KERNEL_TEXT_RODATA_START,
        KERNEL_TEXT_RODATA_SIZE, MEM_ATTR_NORM_NON_CA_BUF);

    /* kernel data region 14 */
    pmp_region_config(PMP_INDEX_KERNEL_DATA_ADDR, PMP_PERMISSION_U_NA_P_RW, KERNEL_DATA_START, KERNEL_DATA_ALIGNED_SIZE,
        MEM_ATTR_NORM_NON_CA_BUF);

    /* the whole tee flash region 15 */
    pmp_region_config(PMP_INDEX_TEE_FLASH_REGION_ADDR, PMP_PERMISSION_U_NA_P_R, SELITEOS_FLASH_REGION_START,
        SELITEOS_FLASH_REGION_LENGTH, MEM_ATTR_WB_RD_ALLOC);

    /* all flash region 18 */
    pmp_region_config(PMP_INDEX_SFC_CODE_ADDR, PMP_PERMISSION_U_NA_P_RX, FLASH_START, FLASH_LENGTH,
        MEM_ATTR_WB_RD_ALLOC);
}

void set_pmp_usr_default(void)
{
    /* drvbox rodata region 0 */
    pmp_region_config(PMP_INDEX_DRVBOX_RODATA_ADDR, PMP_PERMISSION_U_NA_P_R, DRVBOX_RODATA_START,
        DRVBOX_RODATA_ALIGNED_SIZE, MEM_ATTR_NORM_NON_CA_BUF);

    /* common lib text region 1 */
    pmp_region_config(PMP_INDEX_USR_TEXT_ADDR, PMP_PERMISSION_U_X_P_RX, USR_TEXT_START, USR_TEXT_SIZE,
        MEM_ATTR_NORM_NON_CA_BUF);

    /* common lib rodata region 2 */
    pmp_region_config(PMP_INDEX_USR_RODATA_ADDR, PMP_PERMISSION_U_R_P_R, USR_RODATA_START, USR_RODATA_SIZE,
        MEM_ATTR_NORM_NON_CA_BUF);

    /* drvbox text region 3 */
    pmp_region_config(PMP_INDEX_DRVBOX_TEXT_ADDR, PMP_PERMISSION_U_NA_P_RX, DRVBOX_TEXT_START,
        DRVBOX_TEXT_ALIGNED_SIZE, MEM_ATTR_NORM_NON_CA_BUF);

    /* all usrbox data region 5 */
    pmp_region_config(PMP_INDEX_ALL_USRBOX_DATA_ADDR, PMP_PERMISSION_U_NA_P_RW, USRBOX_DATA_START, USRBOX_DATA_SIZE,
        MEM_ATTR_NORM_NON_CA_BUF);

    /* drvbox data region 6 */
    pmp_region_config(PMP_INDEX_DRVBOX_DATA_ADDR, PMP_PERMISSION_U_NA_P_RW, DRVBOX_DATA_START, DRVBOX_DATA_SIZE,
        MEM_ATTR_NORM_NON_CA_BUF);

    /* usr readonly data region 7 */
    pmp_region_config(PMP_INDEX_USR_RO_DATA_ADDR, PMP_PERMISSION_U_R_P_RW, USR_RO_DATA_START, USR_RO_DATA_SIZE,
        MEM_ATTR_NORM_NON_CA_BUF);

    /* common lib data region 8 */
    pmp_region_config(PMP_INDEX_USR_DATA_ADDR, PMP_PERMISSION_U_RW_P_RW, USR_DATA_START, USR_DATA_ALIGNED_SIZE,
        MEM_ATTR_NORM_NON_CA_BUF);
}

#ifdef SELITEOS_KERNEL_TEST
void set_pmp_switch_ree_to_tee(switch_to_ree_type_t type)
{
    switch (type) {
        case NORMAL_TYPE:
            pmp_set_tes(PMP_INDEX_SHARED_DATA_ADDR, 1);
            break;
        case REMOTE_CALL_TYPE:
            /* resume the flash text region to initial state */
            pmp_region_config(PMP_INDEX_FLASH_COMMON_TEXT_ADDR, PMP_PERMISSION_U_X_P_RX, FLASH_COMMON_TEXT_START,
                FLASH_COMMON_TEXT_SIZE, MEM_ATTR_WB_RD_ALLOC);

            pmp_set_tes(PMP_INDEX_SHARED_DATA_ADDR, 1);
            break;
        default:
            break;
    }
}
#endif
