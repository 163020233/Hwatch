/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: Provides sema driver source \n
 */
#include "semactrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "hal_sema.h"

void uapi_sema_init(void)
{
    return sema_porting_init();
}

void uapi_sema_deinit(void)
{
    return sema_porting_deinit();
}

errcode_t uapi_sema_get(uint8_t sema_index, uint32_t wait_time_us)
{
    if (sema_check_index_is_valid(sema_index) == true) {
        return ERRCODE_SEMA_INVALID_INDEX;
    }

    return sema_porting_get(sema_index, wait_time_us);
}

errcode_t uapi_sema_put(uint8_t sema_index)
{
    if (sema_check_index_is_valid(sema_index) == true) {
        return ERRCODE_SEMA_INVALID_INDEX;
    }

    return sema_porting_put(sema_index);
}

errcode_t uapi_sema_force_clear(uint8_t sema_index)
{
    if (sema_check_index_is_valid(sema_index) == true) {
        return ERRCODE_SEMA_INVALID_INDEX;
    }

    return sema_porting_force_clear(sema_index);
}

errcode_t uapi_sema_clear_all(void)
{
    errcode_t ret;
    for (uint8_t i = 0; i < SEMA_BUTT; i++) {
        if (uapi_reg_getbits16(sema_get_sem_status_regaddr(i), SEMA_STS_SUBSYS_MASK, 1) != 0) {
            ret = uapi_sema_put(i);
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
        }
    }
    return ERRCODE_SUCC;
}
