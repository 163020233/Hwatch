/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux service define \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-12-16， Create file. \n
 */
#ifndef UX_SERVICE_DEFINE_H
#define UX_SERVICE_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    UX_SERVICE_BEGIN = 0,             /**< 服务起始 */
    UX_SERVICE_BS = UX_SERVICE_BEGIN, /**< BS服务 */
    UX_SERVICE_AUDIO,                 /**< 音频服务 */
    UX_SERVICE_USB,                   /**< USB服务 */
    UX_SERVICE_END,                   /**< 服务结束 */

    UX_SERVICE_MAX                    /**< 最大值 */
} ux_service_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif