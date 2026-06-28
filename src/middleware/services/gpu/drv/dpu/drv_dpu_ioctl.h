/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description : dpu ioctl head file
 */

#ifndef DRV_GRAPHICS_DRV_DPU_IOCTL_H
#define DRV_GRAPHICS_DRV_DPU_IOCTL_H

#include "td_base.h"
#include "drv_dpu_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define DPU_DEV_NAME  "/dev/dpu"

typedef enum {
    DRV_DPU_CMD_CREATELAYER         = 0,
    DRV_DPU_CMD_DESTROYLAYER        = 1,
    DRV_DPU_CMD_PUTLAYERALPHA       = 2,
    DRV_DPU_CMD_GETLAYERALPHA       = 3,
    DRV_DPU_CMD_PUTLAYERPOS         = 4,
    DRV_DPU_CMD_GETLAYERPOS         = 5,
    DRV_DPU_CMD_PUTLAYERZORDER      = 6,
    DRV_DPU_CMD_GETLAYERZORDER      = 7,
    DRV_DPU_CMD_SHOWLAYER           = 8,
    DRV_DPU_CMD_HIDELAYER           = 9,
    DRV_DPU_CMD_PUTLAYERINFO        = 10,
    DRV_DPU_CMD_GETLAYERINFO        = 11,
    DRV_DPU_CMD_PUTYLAYERCOLORKYE   = 12,
    DRV_DPU_CMD_GETYLAYERCOLORKYE   = 13,
    DRV_DPU_CMD_REFRESHLAYERS       = 14,
    DRV_DPU_CMD_WAITLAYERVSYNC      = 15,
    DRV_DPU_CMD_PUTLAYERVBLANK      = 16,
    DRV_DPU_CMD_GETLAYERVBLANK      = 17,
    DRV_DPU_CMD_QUERYLAYERSTATE     = 18,
    DRV_DPU_CMD_SETOUTPUTINFO       = 19,
    DRV_DPU_CMD_WAITFORDONE         = 20,
    DRV_DPU_CMD_NUM
} drv_dpu_ioctl_cmd;

td_s32 dpu_open(const td_char *path_name, td_s32 flags, td_s32 mode);
td_s32 dpu_ioctl(td_s32 module_idx, td_ulong ioctl_cmd, td_void *para);
td_s32 dpu_close(td_s32 module_idx);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHICS_DRV_DPU_IOCTL_H */
