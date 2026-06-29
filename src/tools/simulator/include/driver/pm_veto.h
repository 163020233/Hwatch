/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides sleep veto header \n
 *
 * History: \n
 * 2023-01-09， Create file. \n
 */

#ifndef PM_VETO_H
#define PM_VETO_H


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_utils_pm_veto PM veto
 * @ingroup  middleware_utils_pm
 * @{
 */

typedef enum pm_veto_id {
    PM_ID_UIKIT               = 0,
    PM_VETO_ID_MAX            = 1,
} pm_veto_id_t;


void uapi_pm_add_sleep_veto(pm_veto_id_t veto_id);

void uapi_pm_remove_sleep_veto(pm_veto_id_t veto_id);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif