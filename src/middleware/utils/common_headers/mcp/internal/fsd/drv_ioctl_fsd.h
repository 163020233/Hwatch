/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: fsd ioctl header
 * Author: AuthorNameMagicTag
 * Create: 2025-11-29
 */

#ifndef __DRV_IOCTL_FSD_H__
#define __DRV_IOCTL_FSD_H__

#include "drv_fsd_struct.h"

typedef struct {
    ext_fsd_alg_coef input_coef;
} fsd_ioctl_init_input_addr;

typedef struct {
    td_s32 handle;
    ext_fsd_task_input_attr input_attr;
} fsd_ioctl_task_input_attr;

typedef struct {
    td_s32 handle;
    ext_fsd_task_output_info output_info;
} fsd_ioctl_task_output_info;

typedef enum {
    IOC_FSD_INIT,
    IOC_FSD_BEGIN_JOB,
    IOC_FSD_ADD_TASK,
    IOC_FSD_END_JOB,
    IOC_FSD_CANCEL_JOB,
    IOC_FSD_DEINIT,
    IOC_FSD_MAX
} ioc_fsd;

#define EXT_FSD_INIT              _IOW(SOC_ID_FSD, IOC_FSD_INIT, fsd_ioctl_init_input_addr)
#define EXT_FSD_BEGIN_JOB         _IOR(SOC_ID_FSD, IOC_FSD_BEGIN_JOB, td_s32)
#define EXT_FSD_ADD_TASK          _IOW(SOC_ID_FSD, IOC_FSD_ADD_TASK, fsd_ioctl_task_input_attr)
#define EXT_FSD_END_JOB           _IOWR(SOC_ID_FSD, IOC_FSD_END_JOB, fsd_ioctl_task_output_info)
#define EXT_FSD_CANCEL_JOB        _IOW(SOC_ID_FSD, IOC_FSD_CANCEL_JOB, td_s32)
#define EXT_FSD_DEINIT            _IOR(SOC_ID_FSD, IOC_FSD_DEINIT, td_s32)
#endif /* __DRV_IOCTL_FSD_H__ */
