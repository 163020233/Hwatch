/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 适配newlib内存接口。
 * Author: LibCK项目组
 * Create: 2025-04-15
 */
#include "sre_attr_external.h"
#include "drb_mem.h"

/*
 * 描述: 申请内存(B类内存，轻核内共享)
 */
OS_SEC_TEXT void *malloc(size_t n)
{
    /* 从底座的默认共享分区中申请，mid固定传0 */
    return DRB_MemAlloc(0, OS_MEM_DEFAULT_PT1, n);
}

/*
 * 描述: 释放内存
 */
OS_SEC_TEXT void free(void *p)
{
    /* 释放回底座的默认共享分区，mid固定传0 */
    (void)DRB_MemFree(0, p);
}
