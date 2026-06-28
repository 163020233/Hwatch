/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2021. All rights reserved.
* Description: error define
* Author: Media Software Group
* Create: 2021-12-07
*/

#ifndef __ERROR_DEF_H__
#define __ERROR_DEF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define APP_ERR_LEVEL_ERROR (4)

/** common error code */
typedef enum {
    APP_ERR_ILLEGAL_PARAM = 0x01,   /* at lease one parameter is illagal
                               * eg, an illegal enumeration value */
    APP_ERR_NULL_PTR = 0x02,        /*  using a NULL point */
    APP_ERR_BUTT,
} AppErrCodeE;

#define ERR_APP_APPID     (0x80000000L + 0x21000000L)

#define APP_DEF_ERR(module, level, errid) \
    ((int32_t)((ERR_APP_APPID) | ((module) << 16) | ((level) << 13) | (errid)))
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* __cplusplus */
#endif  // __ERROR_DEF_H__
