/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 3322 board init source for SELiteOS.
 */
#include "los_typedef.h"
#include "asm/memmap_config.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void os_data_init(void)
{
    (void)memcpy_s(&__kernel_data_start, (UINT32)&__kernel_data_load_size,
        &__kernel_data_load_start, (UINT32)&__kernel_data_load_size);

    (void)memcpy_s(&__usr_data_start, (UINT32)&__usr_data_load_size,
        &__usr_data_load_start, (UINT32)&__usr_data_load_size);

    (void)memcpy_s(&__usr_readonly_data_start, (UINT32)&__usr_readonly_data_load_size,
        &__usr_readonly_data_load_start, (UINT32)&__usr_readonly_data_load_size);

    (void)memcpy_s(&__drvbox_data_start, (UINT32)&__drvbox_data_load_size,
        &__drvbox_data_load_start, (UINT32)&__drvbox_data_load_size);

    (void)memcpy_s(&__boxA_data_start, (UINT32)&__boxA_data_load_size,
        &__boxA_data_load_start, (UINT32)&__boxA_data_load_size);

#if (BOXB_DATA_SIZE != 0)
    (void)memcpy_s(&__boxB_data_start, (UINT32)&__boxB_data_load_size,
        &__boxB_data_load_start, (UINT32)&__boxB_data_load_size);
#endif
}

void os_bss_init(void)
{
    (void)memset_s(&__kernel_bss_start, (UINT32)&__kernel_bss_size, 0, (UINT32)&__kernel_bss_size);

    (void)memset_s(&__usr_bss_start, (UINT32)&__usr_bss_size, 0, (UINT32)&__usr_bss_size);

    (void)memset_s(&__drvbox_bss_start, (UINT32)&__drvbox_bss_size, 0, (UINT32)&__drvbox_bss_size);

    (void)memset_s(&__boxA_bss_start, (UINT32)&__boxA_bss_size, 0, (UINT32)&__boxA_bss_size);

#if (BOXB_DATA_SIZE != 0)
    (void)memset_s(&__boxB_bss_start, (UINT32)&__boxB_bss_size, 0, (UINT32)&__boxB_bss_size);
#endif
}

void ta_stack_init(void)
{
    (void)memset_s(&__boxA_stack_start, (UINT32)&__boxA_stack_size, 0xdf, (UINT32)&__boxA_stack_size);

#if (BOXB_DATA_SIZE != 0)
    (void)memset_s(&__boxB_stack_start, (UINT32)&__boxB_stack_size, 0xdf, (UINT32)&__boxB_stack_size);
#endif
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
