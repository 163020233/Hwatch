/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: alloca.h
 *
 * Create: 2023-01-09
 */

#ifndef	_ALLOCA_H
#define	_ALLOCA_H

#ifdef __cplusplus
extern "C" {
#endif

#define	__NEED_size_t
#include <bits/alltypes.h>

void *alloca(size_t);

#define alloca __builtin_alloca

#ifdef __cplusplus
}
#endif

#endif
