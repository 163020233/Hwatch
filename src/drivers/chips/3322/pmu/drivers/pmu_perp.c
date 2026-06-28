/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides pmu perp interface \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include "pmu_perp.h"
#include "hal_pmu_peripheral.h"

errcode_t uapi_pmu_perp_control(pmu_control_type_t type, uint8_t param)
{
    if (type < PMU_DEV_M_BASE) {
        return ERRCODE_FAIL;
    }
    if (type < PMU_DEV_AON_BASE) {
        hal_pmu_mcpu_perip_config((hal_pmu_mcpu_perips_type_t)(type - PMU_DEV_M_BASE),
                                  (hal_pmu_perip_request_type_t)param);
        return ERRCODE_SUCC;
    }
    if (type < PMU_CONTROL_MAX) {
        hal_pmu_maon_perip_config((hal_pmu_maon_perips_type_t)(type - PMU_DEV_AON_BASE),
                                  (hal_pmu_perip_request_type_t)param);
        return ERRCODE_SUCC;
    }
    return ERRCODE_SUCC;
}