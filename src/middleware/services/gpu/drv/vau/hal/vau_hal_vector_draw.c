/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vau adp vector draw file
 */
#include "vau_hal_vector_draw.h"
#include "platform_core.h"
#include "vau_mem.h"
#include "vau_reg_define.h"

PLT_SEC_ITCM_T td_void vau_hal_vector_draw_mem_init(td_void)
{
    td_u32 et_start, et_end;
    td_u32 et_bk_start, et_bk_end;
    td_u32 et_link_start, et_link_end;

    vau_mem_get_range(VAU_MEM_TYPE_TESSLATION_ET, &et_start, &et_end);
    vau_mem_get_range(VAU_MEM_TYPE_TESSLATION_LINK, &et_link_start, &et_link_end);
    vau_mem_get_range(VAU_MEM_TYPE_TESSLATION_ET_EXT, &et_bk_start, &et_bk_end);

    uapi_reg_write(HAL_VAU_REG_BASEADDR + 0xa6c, et_start);
    uapi_reg_write(HAL_VAU_REG_BASEADDR + 0xa70, et_bk_start);
    uapi_reg_write(HAL_VAU_REG_BASEADDR + 0xa74, et_bk_end - 1);
    uapi_reg_write(HAL_VAU_REG_BASEADDR + 0xa78, et_link_start);
    uapi_reg_write(HAL_VAU_REG_BASEADDR + 0xa7c, et_link_end - 1);
}
