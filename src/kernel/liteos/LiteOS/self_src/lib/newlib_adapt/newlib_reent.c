/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 适配newlib reent接口。
 * Author: LibCK项目组
 * Create: 2025-04-15
 */
#include <reent.h>
#include "sre_attr_external.h"
#include "drb_base.h"

OS_VAL_PERCPU OS_SEC_BSS struct _reent g_reent;

/*
 * 描述: 配合编译器实现C库函数可重入，使用一个TLS的结构体，来替代单线程模式下的全局变量
 */
OS_SEC_TEXT struct _reent *__getreent(void)
{
    return &g_reent;
}
