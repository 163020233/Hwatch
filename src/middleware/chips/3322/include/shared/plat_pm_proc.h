/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: host plat pm header file
 * Author: CompanyName
 */

#ifndef __PLAT_PM_WLAN_H__
#define __PLAT_PM_WLAN_H__

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef void (*pm_plat_suspend_cb)(void);
typedef errcode_t (*pm_plat_resume_cb)(void);

typedef void (*pm_wlan_suspend_cb)(void);
typedef errcode_t (*pm_wlan_resume_cb)(void);

void pm_suspend_service(void);
errcode_t pm_resume_service(void);
void pm_plat_suspend_cb_host_register(pm_plat_suspend_cb cb);
void pm_plat_resume_cb_host_register(pm_plat_resume_cb cb);
void pm_wifi_suspend_cb_host_register(pm_wlan_suspend_cb cb);
void pm_wifi_resume_cb_host_register(pm_wlan_resume_cb cb);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
