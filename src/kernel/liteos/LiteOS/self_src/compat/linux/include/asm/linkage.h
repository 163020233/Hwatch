/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: LiteOS compact for linux
 * Create: 2026-03-25
 */

#ifndef _ASM_LINKAGE_H
#define _ASM_LINKAGE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define __ALIGN .align 0
#define __ALIGN_STR ".align 0"

#define ENDPROC(name)      \
    .type name, %function; \
    END(name)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
