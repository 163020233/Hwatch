/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: header file of math
 * Author: AuthorNameMagicTag
 * Create: 2025-07-23
 */

#ifndef __DRV_MCP_MATH_H__
#define __DRV_MCP_MATH_H__

#include <math.h>
#include "drv_mcp_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifndef MAX2
#define MAX2(x, y)      ((x) > (y) ? (x) : (y))
#endif
#ifndef MIN2
#define MIN2(x, y)      ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX3
#define MAX3(x, y, z)           ((x) > (y) ? MAX2(x, z) : MAX2(y, z))
#endif
#ifndef MIN3
#define MIN3(x, y, z)           ((x) < (y) ? MIN2(x, z) : MIN2(y, z))
#endif

#define clip3(x, min, max)         ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define soc_abs(x)    ((x) >= 0 ? (x) : (-(x)))
#define socalign_up(x, a)       ((((x) + (a) - 1) / (a)) * (a))
#define socalign_down(x, a)     ((x) - (x) % (a))
#define value_between(x, min, max) (((x) >= (min)) && ((x) <= (max)))
    
/* *****************************************************************************
 * rgb(r,g,b)    assemble the r,g,b to 24bit color
 * rgb_r(c)      get RED   from 24bit color
 * rgb_g(c)      get GREEN from 24bit color
 * rgb_b(c)      get BLUE  from 24bit color
 * **************************************************************************** */
#define rgb(r, g, b) ((((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))
#define rgb_r(c) (((c) & 0xff0000) >> 16)
#define rgb_g(c) (((c) & 0xff00) >> 8)
#define rgb_b(c) ((c) & 0xff)
/*
 * yuv(y,u,v)    assemble the y,u,v to 24bit color
 * yuv_y(c)      get Y from 24bit color
 * yuv_u(c)      get U from 24bit color
 * yuv_v(c)      get V from 24bit color
 */
#define yuv(y, u, v) ((((y) & 0xff) << 16) | (((u) & 0xff) << 8) | ((v) & 0xff))
#define yuv_y(c) (((c) & 0xff0000) >> 16)
#define yuv_u(c) (((c) & 0xff00) >> 8)
#define yuv_v(c) ((c) & 0xff)
    
/* *****************************************************************************
 * rgb2yc(r, g, b, *y, *u, *u)    convert r,g,b to y,u,v
 * rgb2yuv(rgb_data, *yuv)             convert rgb to yuv
 * **************************************************************************** */
__inline static td_void rgb2yc(td_u8 r, td_u8 g, td_u8 b, td_u8 *py, td_u8 *pcb, td_u8 *pcr)
{
    /* Y */
    *py = (td_u8)(((r * 66 + g * 129 + b * 25) >> 8) + 16); /* 66 129 25 8 16 表示rgb转yc格式标准公式的参数 */

    /* Cb */
    *pcb = (td_u8)((((b * 112 - r * 38) - g * 74) >> 8) + 128); /* 112 38 74 8 128 表示rgb转yc格式标准公式的参数 */

    /* Cr */
    *pcr = (td_u8)((((r * 112 - g * 94) - b * 18) >> 8) + 128); /* 112 94 18 8 128 表示rgb转yc格式标准公式的参数 */
}

__inline static td_u32 rgb2yuv(td_u32 rgb_data)
{
    td_u8 y, u, v;

    rgb2yc(rgb_r(rgb_data), rgb_g(rgb_data), rgb_b(rgb_data), &y, &u, &v);

    return yuv(y, u, v);
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SOC_MATH_H__ */

