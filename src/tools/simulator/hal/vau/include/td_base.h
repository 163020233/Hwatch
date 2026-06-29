/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __TD_BASE_H__
#define __TD_BASE_H__

#define CONFIG_HIVG_RUN_GCC

#ifdef CONFIG_HIVG_RUN_GCC
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#endif

typedef unsigned char           td_uchar;
typedef unsigned char           td_u8;
typedef unsigned short          td_u16;
typedef unsigned int            td_u32;
typedef unsigned long long      td_u64;
typedef unsigned long           td_ulong;
typedef char                    td_char;
typedef signed char             td_s8;
typedef short                   td_s16;
typedef int                     td_s32;
typedef long long               td_s64;
typedef long                    td_slong;
typedef float                   td_float;
typedef double                  td_double;
typedef void                    td_void;
typedef td_u8                   td_bool;
typedef td_u32                  td_handle;
typedef td_u8                   td_byte;
typedef td_byte*                td_pbyte;
typedef void*                   td_pvoid;
typedef volatile td_u32         td_u32_reg;
typedef unsigned long           td_size_t;
typedef signed   long           td_ssize_t;
typedef unsigned long           td_length_t;

#undef TD_NULL
#define TD_NULL ((void *)0)

#undef EXT_SUCCESS
#define EXT_SUCCESS 0
#undef EXT_FAILURE
#define EXT_FAILURE (-1)

#undef TD_TRUE
#define TD_TRUE 1
#undef TD_FALSE
#define TD_FALSE 0

#ifndef errno_t
typedef int errno_t;
#endif

#ifndef EOK
#define EOK 0
#endif

typedef struct {
    td_s32 x;
    td_s32 y;
    td_u32 width;
    td_u32 height;
} ext_rect;

#endif /* __TD_BASE_H__ */

